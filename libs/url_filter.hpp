#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <cctype>
#include <sstream>
#include <string_view>
#include "../utils/bloom_filter.hpp"  // Make sure this path is correct for your project.
#include <cmph.h>

// Extern declarations for the globals.
extern bloom_filter blocked_domains_filter;
extern bloom_filter blocked_urls_filter;

extern std::vector<std::string> bannedwords_vec;
extern std::vector<std::string> softbanned_vec;
extern std::unordered_set<std::string> bannedSubwords;

// Function declarations.
bool filterUrl(std::string_view fullUri);
void initFiltersOnce();
