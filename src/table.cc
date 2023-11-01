#include "table.h"

Table::Table(const std::string &filename) {
    pager = std::make_shared<Pager>();
}

Table::~Table() {}

void Table::insert(std::shared_ptr<Row> row) {
    if (!pager->getRowsPerPage()) {
        pager->setRowsPerPage(row);
    }
    uint32_t page_num = num_rows / pager->getRowsPerPage();
    // TODO
    // std::shared_ptr<Page> page = pager->getPage();

    std::shared_ptr<Page> page = pager->pages.at(page_num);
    if (page == nullptr) {
        page = pager->pages[page_num] = std::make_shared<Page>();
    }
    page->rows.push_back(row);
}
