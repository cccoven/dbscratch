#ifndef DBSCRATCH_DB_H
#define DBSCRATCH_DB_H

#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <array>
#include <algorithm>
#include <cstring>
#include <map>

enum MetaCommandResult {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
};

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

const size_t COLUMN_USERNAME_SIZE = 32;
const size_t COLUMN_EMAIL_SIZE = 255;

class Row {
public:
    uint32_t id;
    std::string username;
    std::string email;
};

const uint32_t ID_SIZE = sizeof(Row::id);
const uint32_t USERNAME_SIZE = COLUMN_USERNAME_SIZE;
const uint32_t EMAIL_SIZE = COLUMN_EMAIL_SIZE;
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = ID_OFFSET + USERNAME_OFFSET;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;


/*
 * 每页可以存储大小为 4096，假设一行（Row）的大小是 291
 * 那么每页行数（ROWS_PER_PAGE）就是 4096 / 291 = 14
 * 一个 Table 能容纳最多的行数为 (4096 / 291) * 100 = 1407
 * */
const uint32_t PAGE_SIZE = 4096; // 4kb, equal to OS visual memory
const uint32_t TABLE_MAX_PAGES = 100;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

template<typename T>
class Page {
public:
    int index;
    std::array<T, ROWS_PER_PAGE> rows;
};

template<typename T>
class Pager {
public:
    ~Pager();
    
    void flush(uint32_t page_num, uint32_t size);
    
public:
    int fd;
    uint32_t file_len;
    std::array<Page<T> *, TABLE_MAX_PAGES> pages;
};

template<typename T>
class Table {
public:
    Table();
    
    ~Table();

    void storeRow(T t);

public:
    std::string name;
    int num_pages;
    uint32_t num_rows;
    std::shared_ptr<Pager<T>> pager;
};

template<typename T>
class Database {
public:
    Database(std::string &filename);
    
    void open();
    
    void close();

    std::shared_ptr<Table<T>> getTable(std::string &name);
    
public:
    std::string db_filename;
    std::map<std::string, std::shared_ptr<Table<T>>> tables;
};

template<typename T>
class Statement {
public:
    bool beginWith(const std::string &input, const std::string &prefix);

    PrepareResult prepareStatement(const std::string &input);

    PrepareResult prepareInsert(const std::string &input);

    ExecuteResult execute(std::shared_ptr<Table<T>> &table);

    ExecuteResult executeInsert(std::shared_ptr<Table<T>> &table);

    ExecuteResult executeSelect(std::shared_ptr<Table<T>> &table);

public:
    StatementType type;
    Row row_to_insert;
};

#endif // DBSCRATCH_DB_H
