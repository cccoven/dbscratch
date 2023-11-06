#include "table.h"

Table::Table(const std::string &filename) {
    pager = std::make_shared<Pager>(filename);
    std::ifstream inf(filename, std::ios::in | std::ios::binary);
    if (inf.fail()) {
        return;
    }

    inf.seekg(0, std::ios::end);
    uint32_t file_size = inf.tellg();
    pager->file_len = file_size;
    if (file_size > 0) {
        inf.seekg(0, std::ios::beg);
        inf.read((char *) &pager->row_size, sizeof(uint32_t));
        num_rows = file_size / pager->row_size;
    }

    // inf.read((char *) &pager->row_size, sizeof(uint32_t));
    // uint32_t rows_size;
    // inf.read((char *) &rows_size, sizeof(uint32_t));
    
    // uint32_t rows_total = (pager->row_size * rows_size);
    // pager->file_len = rows_total + sizeof(pager->row_size) + sizeof(rows_size);
    // num_rows = rows_total / pager->row_size;

    inf.close();
}

Table::~Table() {}

void Table::insert(std::shared_ptr<Row> row) {
    uint32_t rows_per_page = pager->PAGE_SIZE / row->getSize();
    uint32_t page_num = num_rows / rows_per_page;
    std::shared_ptr<Page> page = pager->getPage(page_num);
    pager->addRow(page_num, row);
}

void Table::persist() {
    // flush
    uint32_t rows_per_page = pager->PAGE_SIZE / pager->row_size;
    uint32_t num_full_pages = num_rows / rows_per_page;
    
    // 如果一页满了，直接写一页
    for (uint32_t i = 0; i < num_full_pages; i++) {
        std::shared_ptr<Page> page = pager->pages.at(i);
        if (page == nullptr) {
            continue;
        }
        pager->flush(i);
    }
    
    // 如果没满一页，则将已有的行数写入
    uint32_t num_additional_rows = num_rows % rows_per_page;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages.at(page_num) != nullptr) {
            pager->flush(page_num);
        }
    }
}
