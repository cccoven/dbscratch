#include <iostream>
#include <fstream>
#include <cstring>
#include <array>

using namespace std;

const size_t SIZE_NAME = 32;
const size_t SIZE_NUMS = 5;

class Student {
public:
    char *serialize() {
        // size_t size_name = name.size() + 1; // 加上 null 终止符
        size_t size_age = sizeof(age);
        size_t size_nums = sizeof(int) * SIZE_NUMS;
        size_t total_size = SIZE_NAME + size_age + size_nums;

        char *buffer = new char[total_size];
        char *current = buffer;
        std::memcpy(current, name, SIZE_NAME);
        current += SIZE_NAME;
        std::memcpy(current, &age, size_age);
        current += size_age;
        std::memcpy(current, &nums, size_nums);

        return buffer;
    }

    void deserialize(const char *buffer) {
        const char *current = buffer;
        std::memcpy(&name, current, SIZE_NAME);
        current += SIZE_NAME;
        std::memcpy(&age, current, sizeof(int));
        current += sizeof(int);
        std::memcpy(&nums, current, sizeof(int) * SIZE_NUMS);
    }

public:
    char name[SIZE_NAME];
    int age;
    array<int, SIZE_NUMS> nums;
};

int main2() {
    Student stu1;
    // stu1.name = "zhangsan";
    const char *name = "zhangsan";
    std::strcpy(stu1.name, name);
    stu1.age = 20;
    for (size_t i = 0; i < SIZE_NUMS; i++) {
        stu1.nums[i] = i * 2;
    }

    char *data = stu1.serialize();

    Student stu2;
    stu2.deserialize(data);

    cout << stu2.name << endl;
    cout << stu2.age << endl;
    for (size_t i = 0; i < stu2.nums.size(); i++) {
        int num = stu2.nums[i];
        cout << num << ", ";
    }
    cout << endl;

    return 0;
}

int main() {
    Student stu1;
    Student stu2;
    ofstream out_file("students.data", ios::out | ios::binary);

    const char *zhangsan = "zhangsan";
    const char *lisi = "lisi";

    strcpy(stu1.name, zhangsan);
    // stu1.name = "zhangsan";
    stu1.age = 18;
    for (size_t i = 0; i < SIZE_NUMS; i++) {
        stu1.nums[i] = i + 1;
    }

    strcpy(stu2.name, lisi);
    // stu2.name = "lisi";
    stu2.age = 20;
    for (size_t i = 0; i < SIZE_NUMS; i++) {
        stu2.nums[i] = (i + 1) * 2;
    }

    char *s1 = stu1.serialize();
    char *s2 = stu2.serialize();
    out_file.write(s1, sizeof(stu1));
    out_file.write(s2, sizeof(stu2));
    out_file.close();


    Student stu;
    ifstream in_file("students.data", ios::in | ios::binary);

    char *buffer = new char[sizeof(stu)];
    while (in_file.read(buffer, sizeof(stu))) {
        stu.deserialize(buffer);
        cout 
            << sizeof(stu.name) 
            << ", " 
            << sizeof(stu.age) 
            << ", " 
            << stu.name 
            << ", " 
            << stu.age 
            << ", ";
        for (size_t i = 0; i < stu.nums.size(); i++) {
            cout << stu.nums[i] << " ";
        }
        cout << endl;
    }
    in_file.close();

    return 0;
}
