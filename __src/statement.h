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

template<typename T>
class Statement {
public:
    ~Statement();

    bool beginWith(const std::string &input, const std::string &prefix);

    PrepareResult prepareStatement(const std::string &input);

    PrepareResult prepareInsert(const std::string &input);

    ExecuteResult execute(std::shared_ptr<Table<T>> &table);

    ExecuteResult executeInsert(std::shared_ptr<Table<T>> &table);

    ExecuteResult executeSelect(std::shared_ptr<Table<T>> &table);

public:
    StatementType type;
    T row_to_insert;
};

template<typename T>
Statement<T>::~Statement() {}

template<typename T>
bool Statement<T>::beginWith(const std::string &input, const std::string &prefix) {
    return input.compare(0, prefix.length(), prefix) == 0;
}

template<typename T>
PrepareResult Statement<T>::prepareInsert(const std::string &input) {
    std::vector<std::string> columns = {"id", "username", "email"};
    std::istringstream assigned_args(input.substr(7));
    std::vector<std::string> values;
    std::string value;

    while (std::getline(assigned_args, value, ' ')) {
        values.push_back(value);
    }

    if (values.size() != columns.size()) {
        return PREPARE_SYNTAX_ERROR;
    }

    if (values.at(0).at(0) == '-') {
        return PREPARE_NEGATIVE_ID;
    }

    Row row{};
    row.id = std::stoul(values.at(0));
    std::string username = values.at(1);
    std::string email = values.at(2);
    if (row.id == 0 || username.empty() || email.empty()) {
        return PREPARE_SYNTAX_ERROR;
    }
    if (username.length() > Row::USERNAME_SIZE || email.length() > Row::EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    std::strcpy(row.username, username.c_str());
    std::strcpy(row.email, email.c_str());

    row_to_insert = row.serialize();

    return PREPARE_SUCCESS;
}

template<typename T>
PrepareResult Statement<T>::prepareStatement(const std::string &input) {
    if (beginWith(input, "insert")) {
        type = STATEMENT_INSERT;
        return prepareInsert(input);
    }
    if (beginWith(input, "select")) {
        type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

template<typename T>
ExecuteResult Statement<T>::execute(std::shared_ptr<Table<T>> &table) {
    switch (type) {
        case STATEMENT_INSERT:
            return executeInsert(table);
        case STATEMENT_SELECT:
            return executeSelect(table);
    }

    return EXECUTE_SUCCESS;
}

template<typename T>
ExecuteResult Statement<T>::executeInsert(std::shared_ptr<Table<T>> &table) {
    // if (table->num_rows >= TABLE_MAX_ROWS) {
    //     return EXECUTE_TABLE_FULL;
    // }
    table->insert(row_to_insert);
    // table->storeRow(row_to_insert);
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

template<typename T>
ExecuteResult Statement<T>::executeSelect(std::shared_ptr<Table<T>> &table) {
    // uint32_t page_num = (table->num_rows / ROWS_PER_PAGE) + 1;
    // for (uint32_t i = 0; i < page_num; i++) {
    //     Page<T> *page = table->pager->getPage(table->num_rows, i);
    //     for (int i = 0; i < page->index; i++) {
    //         Row *row = page->rows[i];
    //         std::cout
    //             << "("
    //             << "id: " << row->id
    //             << " username: " << row->username
    //             << " email: " << row->email
    //             << ")"
    //             << std::endl;
    //     }
    // }

    return EXECUTE_SUCCESS;
}

#endif // DBSCRATCH_STATEMENT_H
