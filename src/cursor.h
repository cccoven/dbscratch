#ifndef DBSCRATCH_CURSOR_H
#define DBSCRATCH_CURSOR_H

#include <memory>

#include "table.h"

enum CursorPosition {
    TABLE_START,
    TABLE_END,
};

class Cursor {
public:
    Cursor() = default;

    Cursor(std::shared_ptr<Table> &tbl, CursorPosition pos);

    Cursor(std::shared_ptr<Table> &tbl, uint32_t row_num, bool eot);

    std::shared_ptr<Row> value();

    void advance();

public:
    std::shared_ptr<Table> table;
    uint32_t row_num;
    bool end_of_table;
};

#endif //DBSCRATCH_CURSOR_H
