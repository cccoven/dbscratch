#include <iostream>
#include <fstream>
#include <cstring>
#include <array>
#include <vector>

using namespace std;

const size_t COLUMN_ID_SIZE = 4;
const size_t COLUMN_USERNAME_SIZE = 32;
const size_t COLUMN_EMAIL_SIZE = 255;

const uint32_t ID_SIZE = COLUMN_ID_SIZE;
const uint32_t USERNAME_SIZE = COLUMN_USERNAME_SIZE;
const uint32_t EMAIL_SIZE = COLUMN_EMAIL_SIZE;
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t NUMS_OFFSET = EMAIL_OFFSET + EMAIL_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

class Row {
public:
    const char *serialize();

    void deserialize(const char *data);

    uint32_t size();

public:
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
    vector<int> nums;
};

const char *Row::serialize() {
    size_t total_size = COLUMN_ID_SIZE + COLUMN_USERNAME_SIZE + COLUMN_EMAIL_SIZE + sizeof(int) * nums.size();
    char *buffer = new char[total_size];
    std::memcpy(buffer + ID_OFFSET, &id, ID_SIZE);
    std::strcpy(buffer + USERNAME_OFFSET, username);
    std::strcpy(buffer + EMAIL_OFFSET, email);
    // 4bytes for the length of nums
    size_t size_nums = nums.size();
    std::memcpy(buffer + NUMS_OFFSET, (char *) &size_nums, sizeof(uint32_t));
    std::memcpy(buffer + NUMS_OFFSET + sizeof(uint32_t), (char *) nums.data(), sizeof(int) * size_nums);
    return buffer;
}

void Row::deserialize(const char *data) {
    std::memcpy(&id, data + ID_OFFSET, ID_SIZE);
    std::strcpy(username, data + USERNAME_OFFSET);
    std::strcpy(email, data + EMAIL_OFFSET);
    uint32_t size_nums;
    std::memcpy(&size_nums, data + NUMS_OFFSET, sizeof(uint32_t));
    // cout << size_nums << ", " << sizeof(uint32_t) << endl;
    const char *data_nums_offset = data + NUMS_OFFSET + sizeof(uint32_t);
    // std::memcpy(&nums, data + NUMS_OFFSET + sizeof(uint32_t), sizeof(int) * size_nums);

    for (int i = 0; i < size_nums; i++) {
        int num;
        std::memcpy(&num, data_nums_offset + i * sizeof(int), sizeof(int));
        nums.push_back(num);
    }
}

uint32_t Row::size() {
    return sizeof(id) + sizeof(username) + sizeof(email) + (sizeof(int) * nums.size());
}

int main() {
    Row row{};
    row.id = 100;
    const char *username = "Melrose";
    std::strcpy(row.username, username);
    const char *email = "foo@bar.com";
    std::strcpy(row.email, email);
    row.nums.push_back(1);
    row.nums.push_back(2);
    row.nums.push_back(3);
    row.nums.push_back(4);
    row.nums.push_back(5);
    row.nums.push_back(6);
    row.nums.push_back(7);
    row.nums.push_back(8);
    row.nums.push_back(9);
    row.nums.push_back(10);

    const char* data = row.serialize();

    Row row2{};
    row2.deserialize(data);

    cout << row2.id << ", " << row2.username << ", " << row2.email << ", ";

    for (size_t i = 0; i < row2.nums.size(); i++) {
        cout << row2.nums.at(i) << " ";
    }
    
    cout << endl;

    cout << sizeof(row.id) << ", " << sizeof(row2.username) << ", " << sizeof(row2.email) << ", " << sizeof(int) * row2.nums.size() << endl;

    cout << "sizeof(row2): " << sizeof(row2) << endl;

    cout << "total: " << row2.size() << endl;

    return 0;
}
