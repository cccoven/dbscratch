#ifndef DBSCRATCH_TABLE_H
#define DBSCRATCH_TABLE_H

#include <string>
#include <memory>
#include <iostream>

#include "pager.h"
#include "row.h"

class Table {
public:
    Table(const std::string &filename);
    
    ~Table();

    void insert(std::shared_ptr<Row> row);

    void persist();

public:
    std::string name;
    int num_pages;
    uint32_t num_rows;
    std::shared_ptr<Pager> pager;
};

#endif // DBSCRATCH_TABLE_H
