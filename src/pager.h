#ifndef DBSCRATCH_PAGER_H
#define DBSCRATCH_PAGER_H

#include <array>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <memory>

#include "row.h"

class Page {
public:
    std::vector<std::shared_ptr<Row>> rows;
};

class Pager {
public:
    Pager() = default;

    Pager(int fd, uint32_t file_len);

    ~Pager();
    
    void flush(uint32_t page_num, uint32_t size);

    std::shared_ptr<Page> getPage(uint32_t num_rows, uint32_t page_num);

    void setRowsPerPage(std::shared_ptr<Row> row);

    uint32_t getRowsPerPage();
    
public:
    static const uint32_t PAGE_SIZE = 4096;
    static const uint32_t TABLE_MAX_PAGES = 100;

    int fd;
    uint32_t file_len;
    std::array<std::shared_ptr<Page>, TABLE_MAX_PAGES> pages;

private:
    uint32_t rows_per_page;
};

#endif // DBSCRATCH_PAGER_H
