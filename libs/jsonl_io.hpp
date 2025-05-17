#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <stdexcept>

namespace rapidwebsift {

// Read a JSONL file line by line and invoke a callback for each line.
template <typename Callback>
void read_jsonl(const std::string& path, Callback cb, std::size_t buf_size = 1 << 16) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Failed to open JSONL file for reading: " + path);
    }
    std::vector<char> buffer(buf_size);
    in.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
    std::string line;
    while (std::getline(in, line)) {
        cb(line);
    }
}

// Write objects to a JSONL file. Each object must be streamable via operator<<.
template <typename Iterable>
void write_jsonl(const std::string& path, const Iterable& objects, std::size_t buf_size = 1 << 16) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("Failed to open JSONL file for writing: " + path);
    }
    std::vector<char> buffer(buf_size);
    out.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
    for (const auto& obj : objects) {
        out << obj << '\n';
    }
}

} // namespace rapidwebsift
