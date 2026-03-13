#include "benchmark.hpp"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    try {
        std::string output_path = "data/results.csv";

        if (argc == 2) {
            output_path = argv[1];
        }
        else if (argc > 2) {
            std::cerr << "Usage: " << argv[0] << " [output_csv]\n";
            return 1;
        }

        const auto results = tlb::RunTlbBenchmark();
        tlb::SaveResultsCsv(output_path, results);

        std::cout << "Benchmark completed. Results saved to: " << output_path << '\n';
        for (const auto& row : results) {
            std::cout << "pages=" << row.pages
                << ", time_ns_per_access=" << row.time_ns_per_access
                << '\n';
        }

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 2;
    }
}