#include "meta_command.h"

MetaCommandResult MetaCommand::execute(const std::string &input, std::shared_ptr<Table> &table) {
    if (input == ".exit") {
        table->persist();
        
        std::exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}
