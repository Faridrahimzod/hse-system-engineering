#include "external_sort.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <file_path>\n";
            return 1;
        }

        const std::string path = argv[1];
        const bool sorted = bigsort::IsFileSorted(path);

        if (sorted) {
            std::cout << "OK\n";
            return 0;
        }

        std::cout << "NOT SORTED\n";
        return 3;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 2;
    }
}