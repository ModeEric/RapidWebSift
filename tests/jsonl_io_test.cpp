#include "../libs/jsonl_io.hpp"
#include <vector>
#include <string>
#include <iostream>

int main() {
    std::vector<std::string> lines = {"{\"a\":1}", "{\"b\":2}"};
    rapidwebsift::write_jsonl("test_tmp.jsonl", lines);

    std::vector<std::string> out;
    rapidwebsift::read_jsonl("test_tmp.jsonl", [&](const std::string& l) {
        out.push_back(l);
    });

    if (out != lines) {
        std::cerr << "Mismatch after read/write" << std::endl;
        return 1;
    }
    std::cout << "JSONL read/write success" << std::endl;
    return 0;
}
