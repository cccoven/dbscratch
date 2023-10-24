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

void Row::serialize(void *slot) {
    std::memcpy((char *) slot + ID_OFFSET, &id, ID_SIZE);
    std::memcpy((char *) slot + USERNAME_OFFSET, username.data(), USERNAME_SIZE);
    std::memcpy((char *) slot + EMAIL_OFFSET, email.data(), EMAIL_SIZE);
}

void Row::deserialize(void *slot, Row &row) {
    std::memcpy(&row.id, (char *) slot + ID_OFFSET, ID_SIZE);
    row.username.assign((char *) slot + USERNAME_OFFSET);
    row.email.assign((char *) slot + EMAIL_OFFSET);
}

Table::~Table() {
    for (void *&page: pages) {
        delete[] (char *) page;
        page = nullptr;
    }
}

void *Table::rowSlot(uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = pages[page_num];
    if (page == nullptr) {
        // 分配一个页
        page = pages[page_num] = new char[PAGE_SIZE];
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return (char *) page + byte_offset;
}

bool Statement::startWith(const std::string &input, const std::string &prefix) {
    return input.compare(0, prefix.length(), prefix) == 0;
}

PrepareResult Statement::prepareStatement(const std::string &input) {
    if (startWith(input, "insert")) {
        type = STATEMENT_INSERT;
        std::istringstream assigned_args(input.substr(7));

        assigned_args >> row_to_insert.id;
        assigned_args >> row_to_insert.username;
        assigned_args >> row_to_insert.email;

        return PREPARE_SUCCESS;
    }
    if (startWith(input, "select")) {
        type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult Statement::execute(std::shared_ptr<Table> &table) {
    switch (type) {
        case STATEMENT_INSERT:
            return executeInsert(table);
        case STATEMENT_SELECT:
            return executeSelect(table);
    }
}

ExecuteResult Statement::executeInsert(std::shared_ptr<Table> &table) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    row_to_insert.serialize(table->rowSlot(table->num_rows));
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

ExecuteResult Statement::executeSelect(std::shared_ptr<Table> &table) {
    Row row{};
    for (uint32_t i = 0; i < table->num_rows; i++) {
        Row::deserialize(table->rowSlot(i), row);
        std::cout
            << "("
            << "id: " << row.id
            << " username: " << row.username
            << " email: " << row.email
            << ")"
            << std::endl;
    }

    return EXECUTE_SUCCESS;
}

int main() {
    std::string command;
    std::shared_ptr<Table> table = std::make_shared<Table>();

    while (true) {
        std::cout << "db > ";
        std::getline(std::cin, command);

        MetaCommand metaCommand{};
        if (command[0] == '.') {
            // consider it's a meta command
            switch (metaCommand.execute(command)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    std::cout << "unrecognized command: " << command << std::endl;
                    continue;
            }
        }

        Statement statement{};
        switch (statement.prepareStatement(command)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_SYNTAX_ERROR:
                std::cout << "syntax error, could not parse statement" << std::endl;
                break;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                std::cout << "unrecognized statement: " << command << std::endl;
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
