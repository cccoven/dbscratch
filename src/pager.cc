#include "pager.h"

Pager::Pager(std::string filename) : filename(filename) {
    fs = std::fstream(filename, std::ios::binary | std::ios::in | std::ios::out);
}

Pager::~Pager() {
    fs.close();
}

void Pager::addRow(uint32_t page_num, std::shared_ptr<Row> row) {
    pages[page_num]->rows.push_back(row);
    if (row_size == 0) {
        row_size = row->getSize();
    }
}

void Pager::flush(uint32_t page_num) {
    std::shared_ptr<Page> page = pages.at(page_num);
    if (page == nullptr) {
        std::cerr << "try to flush null page" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    uint32_t offset = page_num * PAGE_SIZE;
    fs.seekp(offset, std::ios::beg);
    if (fs.fail()) {
        std::cout << "failed to set writing offset: " << offset << std::endl;
    }
    uint32_t curr = fs.tellp();
    std::cout << "current write offset: " << curr << std::endl;

    fs.write((const char *) &row_size, sizeof(uint32_t));
    // 存储 rows 的长度，避免从文件中多读取一次总长度
    uint32_t rows_size = (uint32_t) page->rows.size();
    fs.write((const char *) &rows_size, sizeof(uint32_t));
    for (std::shared_ptr<Row> row: page->rows) {
        fs.write(row->getData(), row->getSize());
    }
    fs.flush();
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

        // page offset
        uint32_t page_offset = page_num * PAGE_SIZE;
        if (page_num <= num_pages && file_len > page_offset) {
        // if (page_num <= num_pages) {
            // skip row_size
            uint32_t offset = page_offset + sizeof(uint32_t);
            fs.seekg(offset, std::ios::beg);
            if (fs.fail()) {
                std::cout << "failed to set reading offset: " << offset << std::endl;
            }
            uint32_t curr = fs.tellg();
            std::cout << "current read offset: " << curr << std::endl;

            // read rows_size
            uint32_t rows_size;
            fs.read((char *) &rows_size, sizeof(uint32_t));

            // read rows
            for (size_t j = 0; j < rows_size; j++) {
                char *data = new char[row_size];
                fs.read(data, row_size);
                // 因为这里不能实例化抽象类，所以 Row 不能使用抽象类
                std::shared_ptr<Row> row = std::make_shared<Row>(data, row_size);
                page->rows.push_back(row);
            }

            // for (uint32_t i = 0; i < num_pages; i++) {
            //     uint32_t page_offset = i * PAGE_SIZE;

            //     // skip row_size
            //     uint32_t offset = page_offset + sizeof(uint32_t);
            //     fs.seekg(offset, std::ios::beg);
            //     if (fs.fail()) {
            //         std::cout << "failed to set reading offset: " << offset << std::endl;
            //     }
            //     uint32_t curr = fs.tellg();
            //     std::cout << "current read offset: " << curr << std::endl;

            //     // read rows_size
            //     uint32_t rows_size;
            //     fs.read((char *) &rows_size, sizeof(uint32_t));

            //     // read rows
            //     for (size_t j = 0; j < rows_size; j++) {
            //         char *data = new char[row_size];
            //         fs.read(data, row_size);
            //         // 因为这里不能实例化抽象类，所以 Row 不能使用抽象类
            //         std::shared_ptr<Row> row = std::make_shared<Row>(data, row_size);
            //         pages.at(i)->rows.push_back(row);
            //     }
            // }
        }
            
        pages[page_num] = page;
    }

    return pages.at(page_num);
}
