#ifndef DBSCRATCH_TABLE_H
#define DBSCRATCH_TABLE_H

#include <string>
#include <memory>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "pager.h"

template<typename T>
class Table {
public:
    Table();
    
    ~Table();

    void storeRow(T t);

    void insert(T t);

public:
    std::string name;
    int num_pages;
    uint32_t num_rows;
    std::shared_ptr<Pager<T>> pager;
};

template<typename T>
Table<T>::Table() {
    int fd = open("/root/projects/dbscratch/test.db", O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | O_EXCL);
    if (fd == -1) {
        std::cerr << "unable to open file: " << errno << std::endl;
        std::exit(EXIT_FAILURE);
    }

    off_t file_len = lseek(fd, 0, SEEK_END);
    pager = std::make_shared<Pager<T>>(fd, file_len);
    num_rows = file_len / ROW_SIZE;
}

template<typename T>
Table<T>::~Table() {}

template<typename T>
void Table<T>::insert(T t) {
    uint32_t page_num = num_rows / ROWS_PER_PAGE;
    // ...
}

template<typename T>
void Table<T>::storeRow(T t) {
    uint32_t page_num = num_rows / ROWS_PER_PAGE;
    Page<T> *page = pager->getPage(page_num);
    page->rows[page->index] = t;
    page->index++;

    // uint32_t page_num = num_rows / ROWS_PER_PAGE;
    // Page<T> *page = pager->pages.at(page_num);
    // if (page == nullptr) {
    
    //     page = pager->pages[page_num] = new Page<T>();
    //     num_pages++;
    // }
    // page->rows[page->index] = t;
    // page->index++;
}

#endif // DBSCRATCH_TABLE_H
