#include "table.h"

Table::Table(const std::string &filename) : num_rows(0) {
    pager = std::make_shared<Pager>(filename);
    std::ifstream inf(filename, std::ios::in | std::ios::binary);
    if (inf.fail()) {
        std::cerr << "failed to open db file: " << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }

    inf.seekg(0, std::ios::end);
    uint32_t file_size = inf.tellg();

    if (!file_size) {
        return;
    }

    pager->file_len = file_size;
    inf.seekg(0, std::ios::beg);
    inf.read((char *) &pager->row_size, sizeof(uint32_t));
    num_rows = file_size / pager->row_size;

    inf.close();
}

Table::~Table() {}

void Table::insert(std::shared_ptr<Row> &row) {
    uint32_t rows_per_page = pager->PAGE_SIZE / row->getSize();
    uint32_t page_num = num_rows / rows_per_page;
    std::shared_ptr<Page> page = pager->getPage(page_num);
    pager->addRow(page_num, row);
}

void Table::persist() {
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
    
    // 如果没满一页，则写入剩余的行数
    uint32_t num_additional_rows = num_rows % rows_per_page;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages.at(page_num) != nullptr) {
            pager->flush(page_num);
        }
    }
}
