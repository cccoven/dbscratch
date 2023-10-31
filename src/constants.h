#ifndef DBSCRATCH_CONSTANTS_H
#define DBSCRATCH_CONSTANTS_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <cstring>

const size_t COLUMN_ID_SIZE = 4;
const size_t COLUMN_USERNAME_SIZE = 32;
const size_t COLUMN_EMAIL_SIZE = 255;

// const uint32_t ID_SIZE = sizeof(Row::id);
const uint32_t ID_SIZE = COLUMN_ID_SIZE;
const uint32_t USERNAME_SIZE = COLUMN_USERNAME_SIZE;
const uint32_t EMAIL_SIZE = COLUMN_EMAIL_SIZE;
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = ID_OFFSET + USERNAME_OFFSET + USERNAME_SIZE;
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

class Row {
public:
    const char *serialize();

    void deserialize(const char *data);

public:
    uint32_t id;
    char username[USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
};

const char *Row::serialize() {
    size_t total_size = COLUMN_ID_SIZE + COLUMN_USERNAME_SIZE + COLUMN_EMAIL_SIZE;
    char *buffer = new char[total_size];
    std::memcpy(buffer + ID_OFFSET, &id, ID_SIZE);
    std::strcpy(buffer + USERNAME_OFFSET, username);
    std::strcpy(buffer + EMAIL_OFFSET, email);
    return buffer;
}

void Row::deserialize(const char *data) {
    std::memcpy(&id, data + ID_OFFSET, ID_SIZE);
    std::strcpy(username, data + USERNAME_OFFSET);
    std::strcpy(email, data + EMAIL_OFFSET);
}

#endif // DBSCRATCH_CONSTANTS_H
