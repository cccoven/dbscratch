#ifndef DBSCRATCH_META_COMMAND_H
#define DBSCRATCH_META_COMMAND_H

#include <string>
#include <memory>
#include <unistd.h>

#include "table.h"

enum MetaCommandResult {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
};

class MetaCommand {
public:
    MetaCommand() = default;

    MetaCommandResult execute(const std::string &input, std::shared_ptr<Table> &table);
};

#endif // DBSCRATCH_META_COMMAND_H
