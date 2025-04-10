#pragma once
#include <vector>

struct WarcRecord {
    std::string_view id;
    std::string_view uri;
    std::string_view date;
    std::string_view payload;
};
int processWarc();
