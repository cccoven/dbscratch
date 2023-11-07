#include "cursor.h"

Cursor::Cursor(std::shared_ptr<Table> &tbl, uint32_t row_num, bool eot) : table(tbl), row_num(row_num), end_of_table(eot) {}

Cursor::Cursor(std::shared_ptr<Table> &tbl, CursorPosition pos) : table(tbl) {
    if (pos == TABLE_START) {
        row_num = 0;
        end_of_table = (table->num_rows == 0);
    } else if (pos == TABLE_END) {
        row_num = table->num_rows;
        end_of_table = true;
    }
}

std::shared_ptr<Row> Cursor::value() {
    std::shared_ptr<Pager> pager = table->pager;
    uint32_t rows_per_page = pager->PAGE_SIZE / pager->row_size;
    uint32_t page_num = row_num / rows_per_page;
    std::shared_ptr<Page> page = pager->getPage(page_num);

    uint32_t row_idx = row_num % rows_per_page;
    return page->rows.at(row_idx);
}

void Cursor::advance() {
    row_num += 1;
    if (row_num == table->num_rows) {
        end_of_table = true;
    }
}
