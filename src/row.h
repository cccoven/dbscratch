#ifndef DBSCRATCH_ROW_H
#define DBSCRATCH_ROW_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <cstring>

class Row {
public:
    Row() = default;

    Row(char *d, uint32_t s);

    void setData(char *d);

    char *getData();

    void setSize(uint32_t s);

    uint32_t getSize();

private:
    char *data;
    uint32_t size;
};

class RowTemp {
public:
    virtual char *serialize() = 0;

    virtual void deserialize(const char *data) = 0;

    virtual uint32_t size() = 0;
};

class User: public RowTemp {
public:
    User() = default;

    char *serialize() override;

    void deserialize(const char *data) override;

    uint32_t size() override;

public:
    static const uint32_t ID_SIZE = sizeof(uint32_t);
    static const uint32_t USERNAME_SIZE = 32;
    static const uint32_t EMAIL_SIZE = 255;
    const uint32_t ID_OFFSET = 0;
    const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
    const uint32_t EMAIL_OFFSET = ID_OFFSET + USERNAME_OFFSET + USERNAME_SIZE;

    uint32_t id;
    char username[USERNAME_SIZE];
    char email[USERNAME_SIZE];
};

#endif // DBSCRATCH_ROW_H
