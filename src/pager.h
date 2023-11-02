#ifndef DBSCRATCH_PAGER_H
#define DBSCRATCH_PAGER_H

#include <array>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>

#include "row.h"

class Page {
public:
    std::vector<std::shared_ptr<Row>> rows;
};

class Pager {
public:
    Pager() = default;

    Pager(std::string filename);

    ~Pager();

    void addRow(uint32_t page_num, std::shared_ptr<Row> row);
    
    void flush(uint32_t page_num);

    std::shared_ptr<Page> getPage(uint32_t page_num);
    
public:
    static const uint32_t PAGE_SIZE = 4096;
    static const uint32_t TABLE_MAX_PAGES = 100;

    std::string filename;
    // std::fstream fs;
    uint32_t file_len;
    uint32_t row_size;
    std::array<std::shared_ptr<Page>, TABLE_MAX_PAGES> pages;
};

#endif // DBSCRATCH_PAGER_H
