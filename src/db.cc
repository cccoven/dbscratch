#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "db.h"

template<typename T>
MetaCommandResult MetaCommand<T>::execute(const std::string &input, std::shared_ptr<Table<T>> &table) {
    if (input == ".exit") {
        // flush
        std::shared_ptr<Pager<T>> pager = table->pager;
        uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

        for (uint32_t i = 0; i < num_full_pages; i++) {
            Page<T> *page = pager->pages.at(i);
            if (page == nullptr) {
                continue;
            }

            pager->flush(i, PAGE_SIZE);
        }

        std::exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

template<typename T>
Pager<T>::Pager(int fd, uint32_t file_len) : fd(fd), file_len(file_len) {}

template<typename T>
Pager<T>::~Pager() {
    for (Page<T> *page: pages) {
        delete page;
    }
}

template<typename T>
void Pager<T>::flush(uint32_t page_num, uint32_t size) {
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

    // for (int i = 0; i < page->index; i++) {
    //     T row = page->rows.at(i);
    //     ssize_t bytes_written = write(fd, row, size);
    //     if (bytes_written == -1) {
    //         std::cerr << "error writing: " << errno << std::endl;
    //         std::exit(EXIT_FAILURE);
    //     }
    // }
}

template<typename T>
Page<T> *Pager<T>::getPage(uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        std::cerr << "tried to fetch page number out of bounds" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    Page<T> *page = pages.at(page_num);
    if (page == nullptr) {
        // cache miss, load from file
        Page<T> *page = new Page<T>();
        uint32_t num_pages = file_len / PAGE_SIZE;

        if (file_len % PAGE_SIZE) {
            // TODO
        }

        if (page_num <= num_pages) {
            // 1 * 4096
            lseek(fd, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(fd, page, PAGE_SIZE);
            if (bytes_read == -1) {
                std::cerr << "error while reading file: " << errno << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }

        pages[page_num] = page;
    }

    return pages.at(page_num);
}

template<typename T>
Table<T>::Table() {
    int fd = open("/root/projects/dbscratch/test.db", O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | O_EXCL);
    if (fd == -1) {
        std::cerr << "unable to open file: " << errno << std::endl;
        std::exit(EXIT_FAILURE);
    }
    // pager = std::make_shared<Pager<T>>(fd, 0);

    off_t file_len = lseek(fd, 0, SEEK_END);
    pager = std::make_shared<Pager<T>>(fd, file_len);
    num_rows = file_len / ROW_SIZE;
}

template<typename T>
Table<T>::~Table() {}

template<typename T>
void Table<T>::storeRow(T t) {
    uint32_t page_num = num_rows / ROWS_PER_PAGE;
    Page<T> *page = pager->getPage(page_num);
    page->rows[page->index] = t;
    page->index++;

    // uint32_t page_num = num_rows / ROWS_PER_PAGE;
    // Page<T> *page = pager->pages.at(page_num);
    // if (page == nullptr) {
    //     // 这里的 page 是一个局部变量
    //     // 使用 new Page<T>() 之后会分配一个新的指针，并不会修改 pages[page_num] 原来指向的指针
    //     // 所以这里需要将 pages[page_num] 指向新的指针
    //     page = pager->pages[page_num] = new Page<T>();
    //     num_pages++;
    // }
    // page->rows[page->index] = t;
    // page->index++;
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
Statement<T>::~Statement() {
    // delete row_to_insert;
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

    Row *row = new Row();
    row->id = std::stoul(values.at(0));
    row->username = values.at(1);
    row->email = values.at(2);

    if (row->id == 0 || row->username.empty() || row->email.empty()) {
        return PREPARE_SYNTAX_ERROR;
    }

    if (row->username.length() > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    if (row->email.length() > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    row_to_insert = row;

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
    for (Page<T> *page: table->pager->pages) {

    }


    // for (int i = 0; i < table->num_pages; i++) {
    //     Page<T> *page = table->pager->pages[i];
    //     for (int j = 0; j < page->index; j++) {
    //         Row *row = page->rows[j];
    //         std::cout
    //                 << "("
    //                 << "id: " << row->id
    //                 << " username: " << row->username
    //                 << " email: " << row->email
    //                 << ")"
    //                 << std::endl;
    //     }
    // }

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
    
    // std::shared_ptr<Database<Row>> db = std::make_shared<Database<Row>>(filename);
    // db->open();

    std::string command;
    std::shared_ptr<Table<Row *>> table = std::make_shared<Table<Row *>>();

    while (true) {
        std::cout << "db > ";
        std::getline(std::cin, command);

        MetaCommand<Row *> metaCommand{};
        if (command.at(0) == '.') {
            // consider it's a meta command
            switch (metaCommand.execute(command, table)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    std::cout << "unrecognized command: " << command << std::endl;
                    continue;
            }
        }

        Statement<Row *> statement{};
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
