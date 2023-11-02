#include "pager.h"

Pager::Pager(std::string filename) : filename(filename) {}

Pager::~Pager() {}

void Pager::addRow(uint32_t page_num, std::shared_ptr<Row> row) {
    pages[page_num]->rows.push_back(row);
    if (row_size == 0) {
        row_size = row->getSize();
    }
}

void Pager::flush(uint32_t page_num) {
    // 直接将一页刷到文件中
    std::shared_ptr<Page> page = pages.at(page_num);
    if (page == nullptr) {
        std::cerr << "try to flush null page" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    uint32_t offset = page_num * PAGE_SIZE;
    std::ofstream outf(filename, std::ios::out | std::ios::binary);
    outf.seekp(offset);
    uint32_t curr = outf.tellp();
    std::cout << "current flush offset: " << curr << std::endl;

    outf.write((const char *) &row_size, sizeof(uint32_t));
    // 存储 rows 的长度，避免从文件中多读取一次总长度
    uint32_t rows_size = (uint32_t) page->rows.size();
    outf.write((const char *) &rows_size, sizeof(uint32_t));
    for (std::shared_ptr<Row> row: page->rows) {
        outf.write(row->getData(), row->getSize());
    }
    outf.close();
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
            // set page offset
            uint32_t page_offset = page_num * PAGE_SIZE;
            // inf.seekg(page_offset, std::ios::beg);
            // if (inf.fail()) {
            //     std::cout << "failed to set reading offset: " << offset << std::endl;
            // }
            // uint32_t curr = inf.tellg();
            // std::cout << "current read offset: " << curr << std::endl;

            for (uint32_t i = 0; i < num_pages; i++) {
                // skip row_size
                uint32_t offset = page_offset + sizeof(uint32_t);
                inf.seekg(offset, std::ios::beg);
                if (inf.fail()) {
                    std::cout << "failed to set reading offset: " << offset << std::endl;
                }
                uint32_t curr = inf.tellg();
                std::cout << "current read offset: " << curr << std::endl;

                // read rows_size
                uint32_t rows_size;
                inf.read((char *) &rows_size, sizeof(uint32_t));

                // read rows
                for (size_t j = 0; j < rows_size; j++) {
                    char *data = new char[row_size];
                    inf.read(data, row_size);
                    // 因为这里不能实例化抽象类，所以 Row 不能使用抽象类
                    std::shared_ptr<Row> row = std::make_shared<Row>(data, row_size);
                    page->rows.push_back(row);
                }
            }
            inf.close();
        }

        pages[page_num] = page;
    }

    return pages.at(page_num);
}
