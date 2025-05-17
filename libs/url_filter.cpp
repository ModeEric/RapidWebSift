#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <cctype>
#include <sstream>
#include <iostream>
#include <cmath>
#include "warc_reader.hpp"
#include "url_filter.hpp"
#include <cmph.h>
#include <stdexcept>
#include <cstdio>

// Global filter structures.
bloom_filter blocked_domains_filter;
bloom_filter blocked_urls_filter;

// Now we store banned words in vectors (for MPHF lookup).
std::vector<std::string> bannedwords_vec;
std::vector<std::string> softbanned_vec;
// Banned subwords remain in an unordered_set.
std::unordered_set<std::string> bannedSubwords;

// MPHF pointers for banned and soft banned words.
cmph_t* bannedwords_mphf = nullptr;
cmph_t* softbanned_mphf = nullptr;

// Streams used during initialization (adjust paths as needed)
std::ifstream blocked_domains_stream("data/urlfilters/domains");
std::ifstream blocked_urls_stream("data/urlfilters/urls");
std::ifstream banned_words_stream("data/banned_words.txt");
std::ifstream banned_subwords_stream("data/banned_subwords.txt");
std::ifstream banned_soft_stream("data/soft_banned_words.txt");

/// Build a minimal perfect hash from a vector of strings and dump it to file.
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmph.h>

cmph_t* buildMPHF(const std::vector<std::string>& keys, const std::string& mphf_file) {
    // Make a copy of keys and remove empty ones:
    std::vector<std::string> filteredKeys = keys;
    filteredKeys.erase(std::remove_if(filteredKeys.begin(), filteredKeys.end(),
                                      [](const std::string &s) { return s.empty(); }),
                       filteredKeys.end());

    // Optionally, remove duplicates.
    std::sort(filteredKeys.begin(), filteredKeys.end());
    filteredKeys.erase(std::unique(filteredKeys.begin(), filteredKeys.end()), filteredKeys.end());

    if (filteredKeys.empty()) {
        throw std::runtime_error("Empty key vector after filtering. Cannot build MPHF.");
    }

    std::cout << "Building MPHF with " << filteredKeys.size() << " keys." << std::endl;

    // Convert vector to C-style strings.
    std::vector<char*> c_keys;
    for (const auto& s : filteredKeys) {
        c_keys.push_back(const_cast<char*>(s.c_str()));
    }

    CMPH_ALGO algo = CMPH_BDZ; // You can try other algorithms if needed.

    // Create the IO adapter from the vector of C strings.
    cmph_io_adapter_t* source = cmph_io_vector_adapter(c_keys.data(), c_keys.size());
    if (!source) {
        throw std::runtime_error("Failed to create IO adapter for MPHF.");
    }

    cmph_config_t* config = cmph_config_new(source);
    if (!config) {
        cmph_io_vector_adapter_destroy(source);
        throw std::runtime_error("Failed to create MPHF config.");
    }
    cmph_config_set_algo(config, algo);

    cmph_t* hash = cmph_new(config);
    // Clean up configuration and adapter.
    cmph_config_destroy(config);
    cmph_io_vector_adapter_destroy(source);

    if (!hash) {
        throw std::runtime_error("Failed to create MPHF.");
    }

    // Dump the hash to file.
    FILE* mphf_fp = fopen(mphf_file.c_str(), "wb");
    if (!mphf_fp) {
        cmph_destroy(hash);
        throw std::runtime_error("Failed to open output MPH file: " + mphf_file);
    }
    cmph_dump(hash, mphf_fp);
    fclose(mphf_fp);

    std::cout << "MPHF successfully built and saved to " << mphf_file << std::endl;
    return hash;
}




/// Initialize filters from original input files.
void initFilter() {
    std::string line;

    // --- Configure bloom filter parameters ---
    bloom_parameters params;
    params.projected_element_count = pow(2, 23); // Estimate
    params.false_positive_probability = 0.01;      // 1% acceptable FPP
    if (!params.compute_optimal_parameters()) {
        std::cerr << "Error computing Bloom filter params\n";
        return;
    }
    blocked_domains_filter = bloom_filter(params);
    blocked_urls_filter = bloom_filter(params);
    
    // --- Insert domains/URLs into bloom filters ---
    while (std::getline(blocked_domains_stream, line)) {
        blocked_domains_filter.insert(line);
    }
    while (std::getline(blocked_urls_stream, line)) {
        blocked_urls_filter.insert(line);
    }
    
    // --- Load banned subwords (unchanged) ---
    while (std::getline(banned_subwords_stream, line)) {
        bannedSubwords.insert(line);
    }
    
    // --- Read banned words and soft banned words into temporary vectors ---
    std::vector<std::string> banned_raw, softbanned_raw;
    while (std::getline(banned_words_stream, line)) {
        banned_raw.push_back(line);
    }
    while (std::getline(banned_soft_stream, line)) {
        softbanned_raw.push_back(line);
    }
    // Save these into global vectors for later lookup.
    bannedwords_vec = banned_raw;
    softbanned_vec = softbanned_raw;
    
    // --- Build MPHF for banned and soft banned words ---
    std::cout << "Number of banned words: " << banned_raw.size() << std::endl;

    bannedwords_mphf = buildMPHF(banned_raw, "data/saved/banned.mphf");
    softbanned_mphf = buildMPHF(softbanned_raw, "data/saved/soft.mphf");
}

/// Load bloom filters and sets from previously saved files.
/// Returns true if loading is successful.
bool loadFilters() {
    // Load Bloom Filters.
    std::ifstream bloom_in("data/saved/filters.bloom", std::ios::binary);
    if (!bloom_in) {
        return false;
    }
    blocked_domains_filter.deserialize(bloom_in);
    blocked_urls_filter.deserialize(bloom_in);
    bloom_in.close();

    // Load the sets.
    std::ifstream set_in("data/saved/filters.txt");
    if (!set_in) {
        return false;
    }
    std::string line;  // Declare the missing 'line' variable.
    enum Section { NONE, BANNED, SUBWORDS, SOFT } section = NONE;
    while (std::getline(set_in, line)) {
        if (line == "#BANNED_WORDS") {
            section = BANNED;
        } else if (line == "#SUBWORDS") {
            section = SUBWORDS;
        } else if (line == "#SOFT") {
            section = SOFT;
        } else if (!line.empty()) {
            switch (section) {
                case BANNED:
                    bannedwords_vec.push_back(line);
                    break;
                case SUBWORDS:
                    bannedSubwords.insert(line);
                    break;
                case SOFT:
                    softbanned_vec.push_back(line);
                    break;
                default:
                    break;
            }
        }
    }
    set_in.close();

    // Load the MPHF for banned words.
    FILE* banned_fp = fopen("data/saved/banned.mphf", "rb");
    if (!banned_fp) return false;
    bannedwords_mphf = cmph_load(banned_fp);
    fclose(banned_fp);
    
    // Load the MPHF for soft banned words.
    FILE* soft_fp = fopen("data/saved/soft.mphf", "rb");
    if (!soft_fp) return false;
    softbanned_mphf = cmph_load(soft_fp);
    fclose(soft_fp);
    
    return true;
}

/// Initialize filters once: load from disk if available, or initialize from scratch.
void initFiltersOnce() {
    if (!loadFilters()) {
        std::cout << "Filters not found on disk. Initializing filters from scratch...\n";
        initFilter();
        // --- Save Bloom filters ---
        std::ofstream bloom_out("data/saved/filters.bloom", std::ios::binary);
        if (!bloom_out) {
            std::cerr << "Error opening filters.bloom for writing.\n";
            return;
        }
        blocked_domains_filter.serialize(bloom_out);
        blocked_urls_filter.serialize(bloom_out);
        bloom_out.close();
        
        // --- Save sets ---
        std::ofstream set_out("data/saved/filters.txt");
        if (!set_out) {
            std::cerr << "Error opening filters.txt for writing.\n";
            return;
        }
        set_out << "#BANNED_WORDS\n";
        for (const auto &word : bannedwords_vec)
            set_out << word << "\n";
        
        set_out << "#SUBWORDS\n";
        for (const auto &word : bannedSubwords)
            set_out << word << "\n";
        
        set_out << "#SOFT\n";
        for (const auto &word : softbanned_vec)
            set_out << word << "\n";
        set_out.close();
    } else {
        std::cout << "Filters loaded successfully from disk.\n";
    }
}

/// Simple normalization: replace non-alnum characters with the provided replacement.
std::string normalize(const std::string_view input, const std::string& replace) {
    std::string output;
    for (char c : input) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            output.push_back(c);
        } else {
            output.append(replace);
        }
    }
    return output;
}

/// Extract the domain from a URL.
std::string_view getDomain(const std::string_view url) {
    size_t pos = url.find("://");
    if (pos != std::string::npos) {
        pos += 3;
    } else {
        pos = 0;
    }
    size_t end = url.find('/', pos);
    return url.substr(pos, end - pos);
}

/// Splits the input string into words (using non-alnum as delimiters)
/// and returns them as lower-case strings in an unordered_set.
std::unordered_set<std::string> splitWords(const std::string_view &input) {
    std::unordered_set<std::string> words;
    std::string current;
    for (char c : input) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            current.push_back(static_cast<char>(std::tolower(c)));
        } else if (!current.empty()) {
            words.insert(current);
            current.clear();
        }
    }
    if (!current.empty()) {
        words.insert(current);
    }
    return words;
}

/// The filtering function: returns true if the URL passes all filters.
bool filterUrl(std::string_view fullUri) {
    std::string_view domain = getDomain(fullUri);
    if (blocked_domains_filter.contains(std::string(domain))) {
        return false; // Blocked domain.
    }
    if (blocked_urls_filter.contains(std::string(fullUri))) {
        return false; // Blocked URL.
    }
    
    // Tokenize the URL.
    auto url_words = splitWords(fullUri);
    
    // Check hard banned words via MPHF lookup.
    for (const auto &word : url_words) {
        if (bannedwords_mphf) {
            unsigned int id = cmph_search(bannedwords_mphf, word.c_str(), word.length());
            if (id < bannedwords_vec.size() && bannedwords_vec[id] == word) {
                return false;
            }
        } else {
            // Fallback: linear search.
            if (std::find(bannedwords_vec.begin(), bannedwords_vec.end(), word) != bannedwords_vec.end()) {
                return false;
            }
        }
    }
    
    // Check soft banned words via MPHF lookup.
    int softCount = 0;
    const int soft_threshold = 2;
    for (const auto &word : url_words) {
        if (softbanned_mphf) {
            unsigned int id = cmph_search(softbanned_mphf, word.c_str(), word.length());
            if (id < softbanned_vec.size() && softbanned_vec[id] == word) {
                ++softCount;
                if (softCount >= soft_threshold)
                    return false;
            }
        } else {
            // Fallback: linear search.
            if (std::find(softbanned_vec.begin(), softbanned_vec.end(), word) != softbanned_vec.end()) {
                ++softCount;
                if (softCount >= soft_threshold)
                    return false;
            }
        }
    }
    
    // Check banned subwords by normalizing URL.
    std::string normalized_url = normalize(fullUri, "_");
    for (const auto &subword : bannedSubwords) {
        if (normalized_url.find(subword) != std::string::npos)
            return false;
    }
    return true; // Passed all filters.
}
