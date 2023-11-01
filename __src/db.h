#ifndef DBSCRATCH_DB_H
#define DBSCRATCH_DB_H

#include <string>
#include <memory>
#include <map>

#include "table.h"

template<typename T>
class Database {
public:
    Database(std::string &filename);
    
    void open();
    
    void close();

    std::shared_ptr<Table<T>> getTable(std::string &name);
    
public:
    std::string db_filename;
    std::map<std::string, std::shared_ptr<Table<T>>> tables;
};

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

#endif // DBSCRATCH_DB_H
