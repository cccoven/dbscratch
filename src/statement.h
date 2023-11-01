#ifndef DBSCRATCH_STATEMENT_H
#define DBSCRATCH_STATEMENT_H

#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <sstream>

#include "table.h"

enum PrepareResult {
    PREPARE_SUCCESS,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LONG,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT,
};

enum StatementType {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
};

enum ExecuteResult {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
};

class Statement {
public:
    ~Statement();

    bool beginWith(const std::string &input, const std::string &prefix);

    PrepareResult prepareStatement(const std::string &input);

    PrepareResult prepareInsert(const std::string &input);

    ExecuteResult execute(std::shared_ptr<Table> &table);

    ExecuteResult executeInsert(std::shared_ptr<Table> &table);

    ExecuteResult executeSelect(std::shared_ptr<Table> &table);

public:
    StatementType type;
    std::shared_ptr<Row> row_to_insert;
};

#endif // DBSCRATCH_STATEMENT_H
