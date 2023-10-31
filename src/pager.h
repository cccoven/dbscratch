#ifndef DBSCRATCH_PAGER_H
#define DBSCRATCH_PAGER_H

#include <array>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#include "constants.h"

template<typename T>
class Page {
public:
    int index;
    std::array<T, ROWS_PER_PAGE> rows;

private:
    size_t rows_per_page;
};

template<typename T>
class Pager {
public:
    Pager(int fd, uint32_t file_len);

    ~Pager();
    
    void flush(uint32_t page_num, uint32_t size);

    Page<T> *getPage(uint32_t num_rows, uint32_t page_num);
    
public:
    int fd;
    uint32_t file_len;
    std::array<Page<T> *, TABLE_MAX_PAGES> pages;
};

template<typename T>
Pager<T>::Pager(int fd, uint32_t file_len) : fd(fd), file_len(file_len) {}

template<typename T>
Pager<T>::~Pager() {
    for (Page<T> *page: pages) {
        delete page;
    }
}

template<typename T>
void Pager<T>::flush(uint32_t page_num, uint32_t size) {
    Page<T> *page = pages.at(page_num);
    if (page == nullptr) {
        std::cerr << "try to flush null page" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    off_t offset = lseek(fd, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        std::cerr << "error seeking: " << errno << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    // ssize_t bytes_written = write(fd, page->rows.data(), size);
    // if (bytes_written == -1) {
    //     std::cerr << "error writing: " << errno << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    for (int i = 0; i < page->index; i++) {
        T row = page->rows.at(i);
        off_t ofs = lseek(fd, i * ROW_SIZE, SEEK_SET);
        // ssize_t bytes_written = write(fd, row, size);
        ssize_t bytes_written = write(fd, row, ROW_SIZE);
        if (bytes_written == -1) {
            std::cerr << "error writing: " << errno << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

template<typename T>
Page<T> *Pager<T>::getPage(uint32_t num_rows, uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        std::cerr << "tried to fetch page number out of bounds" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    Page<T> *page = pages.at(page_num);
    if (page == nullptr) {
        // cache miss, load from file
        page = new Page<T>();
        uint32_t num_pages = file_len / PAGE_SIZE;

        if (file_len % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            // 0 * 4096
            // lseek(fd, page_num * PAGE_SIZE, SEEK_SET);

            for (uint32_t i = 0; i < num_rows; i++) {
                // T row = page->rows[i];
                if (page->rows[i] == nullptr) {
                    page->rows[i] = new Row();
                }
                
                off_t offset = lseek(fd, i * ROW_SIZE, SEEK_SET);
                ssize_t bytes_read = read(fd, page->rows[i], ROW_SIZE);
                if (bytes_read == -1) {
                    std::cerr << "error while reading file: " << errno << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                page->index++;
                // page->rows[page->index] = row;
            }

            // ssize_t bytes_read = read(fd, page->rows.data(), num_rows * ROW_SIZE);
            // if (bytes_read == -1) {
            //     std::cerr << "error while reading file: " << errno << std::endl;
            //     std::exit(EXIT_FAILURE);
            // }
        }

        // 这里的 page 是一个局部变量
        // 使用 new Page<T>() 之后会分配一个新的指针，并不会修改 pages[page_num] 原来指向的指针
        // 所以这里需要将 pages[page_num] 指向新的指针
        pages[page_num] = page;
    }

    return pages.at(page_num);
}

#endif // DBSCRATCH_PAGER_H
