#include "row.h"

Row::Row(char *d, uint32_t s) : data(d), size(s) {}

void Row::setData(char *d) {
    data = d;
}

char *Row::getData() {
    return data;
}

void Row::setSize(uint32_t s) {
    size = s;
}

uint32_t Row::getSize() {
    return size;
}

char *User::serialize() {
    char *buffer = new char[size()];
    std::memcpy(buffer + ID_OFFSET, &id, ID_SIZE);
    std::strcpy(buffer + USERNAME_OFFSET, username);
    std::strcpy(buffer + EMAIL_OFFSET, email);
    return buffer;
}

void User::deserialize(const char *data) {
    std::memcpy(&id, data + ID_OFFSET, ID_SIZE);
    std::strcpy(username, data + USERNAME_OFFSET);
    std::strcpy(email, data + EMAIL_OFFSET);
}

uint32_t User::size() {
    return ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
}
