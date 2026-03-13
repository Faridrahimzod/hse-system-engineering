#include "types.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " <file_path> <count>\n";
            return 1;
        }

        const std::string path = argv[1];
        const std::size_t count = std::stoull(argv[2]);

        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        if (!out) {
            throw std::runtime_error("Failed to open output file");
        }

        std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<bigsort::Value> dist(
            std::numeric_limits<bigsort::Value>::min(),
            std::numeric_limits<bigsort::Value>::max());

        for (std::size_t i = 0; i < count; ++i) {
            const bigsort::Value value = dist(rng);
            out.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }

        if (!out) {
            throw std::runtime_error("Failed to write generated data");
        }

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 2;
    }
}