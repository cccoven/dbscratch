#include <iostream>
#include <string>
#include <sstream>

enum MetaCommandResult {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
};

enum PrepareResult {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
};

enum StatementType {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
};

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

const size_t COLUMN_USERNAME_SIZE = 32;
const size_t COLUMN_EMAIL_SIZE = 255;

class Row {
    public:
        Row() = default;

    public:
        uint32_t id;
        std::string username;
        std::string email;
};

class Statement {
public:
    Statement() = default;

    bool startWith(const std::string &input, const std::string &prefix);

    PrepareResult prepareStatement(const std::string &input);

    void execute();

public:
    StatementType type;
    Row row_to_insert;
};

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

void Statement::execute() {
    switch (type) {
        case STATEMENT_INSERT:
            std::cout << "do insert" << std::endl;
            break;
        case STATEMENT_SELECT:
            std::cout << "do select" << std::endl;
            break;
    }
}


const uint32_t PAGE_SIZE = 4096;
const uint32_t TABLE_MAX_PAGES = 100;
// const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

class Table {
public:
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
};

int main() {
    std::string command;

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
            case PREPARE_UNRECOGNIZED_STATEMENT:
                std::cout << "unrecognized statement: " << command << std::endl;
                continue;
        }

        statement.execute();
        std::cout << "executed" << std::endl;
    }
}
