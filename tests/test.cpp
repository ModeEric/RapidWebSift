#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include "src/warc_reader.hpp"
#include <iostream>
#include "src/url_filter.hpp" 

int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    auto t1 = high_resolution_clock::now();

    

    initFiltersOnce();

    int res = processWarc();
    auto t2 = high_resolution_clock::now();
    std::cout<<res<<std::endl;
    duration<double,std::milli> ms_double = t2-t1;
    std::cout << ms_double.count() << std::endl;
    return 0;
}