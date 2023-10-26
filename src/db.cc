#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "db.h"

class MetaCommand {
public:
    MetaCommand() = default;

    MetaCommandResult execute(const std::string &input);
};

MetaCommandResult MetaCommand::execute(const std::string &input) {
    if (input == ".exit") {
        std::exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

template<typename T>
Pager<T>::~Pager() {
    for (Page<T> *page: pages) {
        delete page;
    }
}

template<typename T>
void Pager<T>::flush(uint32_t page_num, uint32_t size) {
    // mock
    if (fd == 0) {
        fd = open("/Users/coven/VSCodeProjects/dbscratch/test.db", O_RDWR | O_CREAT | S_IWUSR | S_IRUSR);
        if (fd == -1) {
            std::cerr << "unable to open file" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    
    Page<T> *page = pages.at(page_num);
    if (page == nullptr) {
        std::cerr << "try to flush null page" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    off_t offset = lseek(fd, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        std::cerr << "error seeking: " << errno << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    ssize_t bytes_written = write(fd, page, size);
    if (bytes_written == -1) {
        std::cerr << "error writing: " << errno << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

template<typename T>
Table<T>::Table() {
    pager = std::make_shared<Pager<T>>();
}

template<typename T>
Table<T>::~Table() {}

template<typename T>
void Table<T>::storeRow(T t) {
    uint32_t page_num = num_rows / ROWS_PER_PAGE;
    Page<T> *page = pager->pages[page_num];
    if (page == nullptr) {
        // 这里的 page 是一个局部变量
        // 使用 new Page<T>() 之后会分配一个新的指针，并不会修改 pages[page_num] 原来指向的指针
        // 所以这里需要将 pages[page_num] 指向新的指针
        page = pager->pages[page_num] = new Page<T>();
        num_pages++;
    }
    page->rows[page->index] = t;
    page->index++;
    
    // simple flush
    pager->flush(page_num, PAGE_SIZE);
}

template<typename T>
Database<T>::Database(std::string &filename) : db_filename(filename) {}

template<typename T>
void Database<T>::open() {}

template<typename T>
void Database<T>::close() {}

template<typename T>
std::shared_ptr<Table<T>> Database<T>::getTable(std::string &name) {
    std::shared_ptr<Table<T>> table = tables.at(name);
    return table;
}

template<typename T>
bool Statement<T>::beginWith(const std::string &input, const std::string &prefix) {
    return input.compare(0, prefix.length(), prefix) == 0;
}

template<typename T>
PrepareResult Statement<T>::prepareInsert(const std::string &input) {
    type = STATEMENT_INSERT;
    std::vector<std::string> columns = {"id", "username", "email"};
    std::istringstream assigned_args(input.substr(7));
    std::vector<std::string> values;
    std::string value;

    while (std::getline(assigned_args, value, ' ')) {
        values.push_back(value);
    }

    if (values.size() != columns.size()) {
        return PREPARE_SYNTAX_ERROR;
    }

    if (values.at(0).at(0) == '-') {
        return PREPARE_NEGATIVE_ID;
    }

    row_to_insert.id = std::stoul(values.at(0));
    row_to_insert.username = values.at(1);
    row_to_insert.email = values.at(2);

    if (row_to_insert.id == 0 || row_to_insert.username.empty() || row_to_insert.email.empty()) {
        return PREPARE_SYNTAX_ERROR;
    }

    if (row_to_insert.username.length() > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    if (row_to_insert.email.length() > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    return PREPARE_SUCCESS;
}

template<typename T>
PrepareResult Statement<T>::prepareStatement(const std::string &input) {
    if (beginWith(input, "insert")) {
        return prepareInsert(input);
    }
    if (beginWith(input, "select")) {
        type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

template<typename T>
ExecuteResult Statement<T>::execute(std::shared_ptr<Table<T>> &table) {
    switch (type) {
        case STATEMENT_INSERT:
            return executeInsert(table);
        case STATEMENT_SELECT:
            return executeSelect(table);
    }

    return EXECUTE_SUCCESS;
}

template<typename T>
ExecuteResult Statement<T>::executeInsert(std::shared_ptr<Table<T>> &table) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    table->storeRow(row_to_insert);
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

template<typename T>
ExecuteResult Statement<T>::executeSelect(std::shared_ptr<Table<T>> &table) {
    for (int i = 0; i < table->num_pages; i++) {
        Page<T> *page = table->pager->pages[i];
        for (int j = 0; j < page->index; j++) {
            Row row = page->rows[j];
            std::cout
                    << "("
                    << "id: " << row.id
                    << " username: " << row.username
                    << " email: " << row.email
                    << ")"
                    << std::endl;
        }
    }

    return EXECUTE_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "database file is required" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string filename;
    filename.assign(argv[1]);
    
    std::cout << "db file: " << filename << std::endl;
    
    std::shared_ptr<Database<Row>> db = std::make_shared<Database<Row>>(filename);
    db->open();

    std::string command;
    std::shared_ptr<Table<Row>> table = std::make_shared<Table<Row>>();

    while (true) {
        std::cout << "db > ";
        std::getline(std::cin, command);

        MetaCommand metaCommand{};
        if (command.at(0) == '.') {
            // consider it's a meta command
            switch (metaCommand.execute(command)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    std::cout << "unrecognized command: " << command << std::endl;
                    continue;
            }
        }

        Statement<Row> statement{};
        switch (statement.prepareStatement(command)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_NEGATIVE_ID:
                std::cerr << "id must be positive" << std::endl;
                continue;
            case PREPARE_STRING_TOO_LONG:
                std::cerr << "string is too long" << std::endl;
                continue;
            case PREPARE_SYNTAX_ERROR:
                std::cerr << "syntax error, could not parse statement" << std::endl;
                continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                std::cerr << "unrecognized statement: " << command << std::endl;
                continue;
        }

        switch (statement.execute(table)) {
            case EXECUTE_SUCCESS:
                std::cout << "executed" << std::endl;
                break;
            case EXECUTE_TABLE_FULL:
                std::cerr << "table is full" << std::endl;
                break;
        }
    }
}
