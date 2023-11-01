#ifndef DBSCRATCH_DB_H
#define DBSCRATCH_DB_H

#include <string>
#include <memory>
#include <map>

#include "table.h"

class Database {
public:
    Database(std::string &filename);
    
    void open();
    
    void close();

    std::shared_ptr<Table> getTable(std::string &name);
    
public:
    std::string db_filename;
    std::map<std::string, std::shared_ptr<Table>> tables;
};

#endif // DBSCRATCH_DB_H
