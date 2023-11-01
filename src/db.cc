#include "db.h"

Database::Database(std::string &filename) : db_filename(filename) {}

void Database::open() {}

void Database::close() {}

std::shared_ptr<Table> Database::getTable(std::string &name) {
    std::shared_ptr<Table> table = tables.at(name);
    return table;
}
