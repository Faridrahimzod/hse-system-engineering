#include "external_sort.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {
    try {
        if (argc != 2 && argc != 3) {
            std::cerr << "Usage: " << argv[0] << " <file_path> [memory_limit_mb]\n";
            return 1;
        }

        const std::string path = argv[1];
        std::size_t memory_limit_bytes = 0;

        if (argc == 3) {
            const std::size_t memory_limit_mb = std::stoull(argv[2]);
            memory_limit_bytes = memory_limit_mb * 1024ull * 1024ull;
        }
        else {
            memory_limit_bytes = bigsort::DefaultMemoryLimitBytes(path);
        }

        bigsort::SortFile(path, memory_limit_bytes);
        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 2;
    }
}