#include <iostream>
#include <string>
#include <memory>

#include "meta_command.h"
#include "statement.h"

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
    std::shared_ptr<Table<char *>> table = std::make_shared<Table<char *>>(filename);

    while (true) {
        std::cout << "db > ";
        std::getline(std::cin, command);

        MetaCommand<char *> metaCommand{};
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

        Statement<char *> statement{};
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
