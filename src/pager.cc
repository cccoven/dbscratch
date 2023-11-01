#include "pager.h"

Pager::Pager(int fd, uint32_t file_len, std::string filename) : fd(fd), file_len(file_len), filename(filename) {
    
}

Pager::~Pager() {}

void Pager::setRowSize(uint32_t s) {
    row_size = s;
}

uint32_t Pager::getRowSize() {
    return row_size;
}

void Pager::setRowsPerPage(std::shared_ptr<Row> row) {
    rows_per_page = PAGE_SIZE / row->size();
}

uint32_t Pager::getRowsPerPage() {
    return rows_per_page;
}

void Pager::flush(uint32_t page_num, uint32_t size) {
    // TODO 直接将一页刷到文件中
    std::shared_ptr<Page> page = pages.at(page_num);
    if (page == nullptr) {
        std::cerr << "try to flush null page" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::ofstream outf(filename, std::ios::out | std::ios::binary);
    outf.write((const char *) &row_size, sizeof(uint32_t));
    // 存储 rows 的长度，避免从文件中多读取一次总长度
    uint32_t rows_size = (uint32_t) page->rows.size();
    outf.write((const char *) &rows_size, sizeof(uint32_t));
    for (std::shared_ptr<Row> row: page->rows) {
        outf.write(row->serialize(), row->size());
    }
    outf.close();

    // std::shared_ptr<Page> page = pages.at(page_num);
    // if (page == nullptr) {
    //     std::cerr << "try to flush null page" << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    // off_t offset = lseek(fd, page_num * PAGE_SIZE, SEEK_SET);
    // if (offset == -1) {
    //     std::cerr << "error seeking: " << errno << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    // ssize_t bytes_written = write(fd, page->rows.data(), size);
    // if (bytes_written == -1) {
    //     std::cerr << "error writing: " << errno << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    // for (int i = 0; i < page->index; i++) {
    //     T row = page->rows.at(i);
    //     off_t ofs = lseek(fd, i * ROW_SIZE, SEEK_SET);
    //     // ssize_t bytes_written = write(fd, row, size);
    //     ssize_t bytes_written = write(fd, row, ROW_SIZE);
    //     if (bytes_written == -1) {
    //         std::cerr << "error writing: " << errno << std::endl;
    //         std::exit(EXIT_FAILURE);
    //     }
    // }
}

std::shared_ptr<Page> Pager::getPage(uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        std::cerr << "tried to fetch page number out of bounds" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::shared_ptr<Page> page = pages.at(page_num);
    if (page == nullptr) {
        page = std::make_shared<Page>();
        uint32_t num_pages = file_len / PAGE_SIZE;

        if (file_len % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            std::ifstream inf(filename, std::ios::in | std::ios::binary);
            for (uint32_t i = 0; i < num_pages; i++) {
                inf.seekg(sizeof(uint32_t));
                uint32_t rows_size;
                inf.read((char *) &rows_size, sizeof(uint32_t));
                for (size_t j = 0; j < rows_size; j++) {
                    char *data = new char[row_size];
                    inf.read(data, row_size);
                    // TODO 这里不能实例化创建抽象类
                    std::shared_ptr<User> row = std::make_shared<User>();
                    row->deserialize(data);
                    page->rows.push_back(row);
                }
            }
        }

        pages[page_num] = page;
    }

    return pages.at(page_num);
}
