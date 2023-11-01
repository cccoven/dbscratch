#include "table.h"

Table::Table(const std::string &filename) {
    pager = std::make_shared<Pager>(0, 0, filename);
    std::ifstream inf(filename, std::ios::in | std::ios::binary);
    if (inf.fail()) {
        return;
    }
    inf.read((char *) &pager->row_size, sizeof(uint32_t));
    uint32_t rows_size;
    inf.read((char *) &rows_size, sizeof(uint32_t));
    
    pager->file_len = (pager->row_size * rows_size);
    num_rows = pager->file_len / pager->row_size;

    inf.close();
}

Table::~Table() {}

void Table::insert(std::shared_ptr<Row> row) {
    // if (!pager->getRowsPerPage()) {
    //     pager->setRowsPerPage(row);
    // }
    if (!pager->getRowSize()) {
        pager->setRowSize(row->size());
    }
    uint32_t page_num = num_rows / (pager->PAGE_SIZE / row->size());
    // TODO
    // std::shared_ptr<Page> page = pager->getPage(page_num);

    // pager->addRow(row);

    std::shared_ptr<Page> page = pager->pages.at(page_num);
    if (page == nullptr) {
        page = pager->pages[page_num] = std::make_shared<Page>();
    }

    page->rows.push_back(row);
}
