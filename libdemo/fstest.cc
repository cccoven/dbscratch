#include <iostream>
#include <fstream>

using namespace std;

int main() {
    fstream fs;
    // fstream fs("test.data", ios::out | ios::binary);

    fs = fstream("test.data", ios::binary | ios::in | ios::out);

    const char *str1 = "hello";
    const char *str2 = "world";

    fs.write(str1, 5);

    fs.seekp(4096, ios::beg);
    if (fs.fail()) {
        std::cout << "failed to set flush offset: " << fs.fail() << std::endl;
    }
    uint32_t curr = fs.tellp();
    std::cout << "current flush offset: " << curr << std::endl;


    fs.write(str2, 5);

    fs.close();

    // fstream fs2("test.data", ios::out | ios::binary);

    // fs2.seekp(5);

    // fs2.write(str2, 5);

    // fs.flush();

    // fs2.close();

    return 0;
}
