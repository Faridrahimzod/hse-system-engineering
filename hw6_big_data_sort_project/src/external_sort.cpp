#include "external_sort.hpp"

#include "file_mapping.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <queue>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace bigsort {
    namespace fs = std::filesystem;

    namespace {

        std::uintmax_t FileSizeChecked(const std::string& path) {
            std::error_code ec;
            auto size = fs::file_size(path, ec);
            if (ec) {
                throw std::runtime_error("Failed to get file size: " + path);
            }
            return size;
        }

        std::size_t ClampChunkValues(std::size_t memory_limit_bytes) {
            std::size_t values = memory_limit_bytes / sizeof(Value);
            return std::max<std::size_t>(values, 1);
        }

        std::string ChunkPath(const fs::path& base_dir, const std::string& base_name, std::size_t index) {
            return (base_dir / (base_name + ".chunk." + std::to_string(index))).string();
        }

        std::string MergedPath(const fs::path& base_dir, const std::string& base_name) {
            return (base_dir / (base_name + ".sorted")).string();
        }

        void WriteVectorToFile(const std::string& path, const std::vector<Value>& data) {
            std::ofstream out(path, std::ios::binary | std::ios::trunc);
            if (!out) {
                throw std::runtime_error("Failed to open output file: " + path);
            }
            out.write(reinterpret_cast<const char*>(data.data()),
                static_cast<std::streamsize>(data.size() * sizeof(Value)));
            if (!out) {
                throw std::runtime_error("Failed to write file: " + path);
            }
        }

        std::vector<std::string> SplitAndSortChunks(const std::string& path, std::size_t memory_limit_bytes) {
            const std::uintmax_t file_size = FileSizeChecked(path);
            if (file_size % sizeof(Value) != 0) {
                throw std::runtime_error("Input file size is not divisible by sizeof(int64_t)");
            }

            const std::size_t chunk_values = ClampChunkValues(memory_limit_bytes);
            const std::size_t chunk_bytes = chunk_values * sizeof(Value);

            fs::path input_path(path);
            fs::path dir = input_path.parent_path().empty() ? fs::current_path() : input_path.parent_path();
            const std::string base_name = input_path.filename().string();

            std::vector<std::string> chunks;
            std::size_t offset = 0;
            std::size_t index = 0;

            while (offset < file_size) {
                const std::size_t current_bytes = static_cast<std::size_t>(
                    std::min<std::uintmax_t>(chunk_bytes, file_size - offset));

                ReadOnlyFileMapping mapping(path, offset, current_bytes);

                const Value* begin = reinterpret_cast<const Value*>(mapping.Data());
                const std::size_t count = current_bytes / sizeof(Value);

                std::vector<Value> values(begin, begin + count);
                std::sort(values.begin(), values.end());

                std::string chunk_path = ChunkPath(dir, base_name, index++);
                WriteVectorToFile(chunk_path, values);
                chunks.push_back(std::move(chunk_path));

                offset += current_bytes;
            }

            return chunks;
        }

        struct ChunkReader {
            std::ifstream in;
            Value current{ 0 };
            bool has_value{ false };

            explicit ChunkReader(const std::string& path)
                : in(path, std::ios::binary) {
                if (!in) {
                    throw std::runtime_error("Failed to open chunk: " + path);
                }
                Advance();
            }

            void Advance() {
                in.read(reinterpret_cast<char*>(&current), sizeof(Value));
                has_value = static_cast<bool>(in);
            }
        };

        struct HeapNode {
            Value value{ 0 };
            std::size_t index{ 0 };

            bool operator>(const HeapNode& other) const {
                return value > other.value;
            }
        };

        void MergeChunks(const std::vector<std::string>& chunks, const std::string& output_path) {
            std::vector<ChunkReader> readers;
            readers.reserve(chunks.size());

            for (const auto& chunk : chunks) {
                readers.emplace_back(chunk);
            }

            std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>> heap;

            for (std::size_t i = 0; i < readers.size(); ++i) {
                if (readers[i].has_value) {
                    heap.push(HeapNode{ readers[i].current, i });
                }
            }

            std::ofstream out(output_path, std::ios::binary | std::ios::trunc);
            if (!out) {
                throw std::runtime_error("Failed to open merge output: " + output_path);
            }

            while (!heap.empty()) {
                HeapNode node = heap.top();
                heap.pop();

                out.write(reinterpret_cast<const char*>(&node.value), sizeof(Value));
                if (!out) {
                    throw std::runtime_error("Failed during merge write");
                }

                auto& reader = readers[node.index];
                reader.Advance();
                if (reader.has_value) {
                    heap.push(HeapNode{ reader.current, node.index });
                }
            }
        }

    }  // namespace

    std::size_t DefaultMemoryLimitBytes(const std::string& path) {
        const std::uintmax_t file_size = FileSizeChecked(path);
        const std::uintmax_t limit = std::max<std::uintmax_t>(file_size / 10, sizeof(Value));
        return static_cast<std::size_t>(limit);
    }

    void SortFile(const std::string& path, std::size_t memory_limit_bytes) {
        if (memory_limit_bytes < sizeof(Value)) {
            throw std::runtime_error("Memory limit is too small");
        }

        const auto chunks = SplitAndSortChunks(path, memory_limit_bytes);
        if (chunks.empty()) {
            return;
        }

        fs::path input_path(path);
        fs::path dir = input_path.parent_path().empty() ? fs::current_path() : input_path.parent_path();
        const std::string merged_path = MergedPath(dir, input_path.filename().string());

        try {
            if (chunks.size() == 1) {
                fs::rename(chunks.front(), merged_path);
            }
            else {
                MergeChunks(chunks, merged_path);
            }

            fs::rename(merged_path, path);

            for (const auto& chunk : chunks) {
                std::error_code ec;
                fs::remove(chunk, ec);
            }
        }
        catch (...) {
            std::error_code ec;
            fs::remove(merged_path, ec);
            for (const auto& chunk : chunks) {
                fs::remove(chunk, ec);
            }
            throw;
        }
    }

    bool IsFileSorted(const std::string& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) {
            throw std::runtime_error("Failed to open file for verification");
        }

        Value prev{};
        Value current{};

        in.read(reinterpret_cast<char*>(&prev), sizeof(Value));
        if (!in) {
            return true;
        }

        while (in.read(reinterpret_cast<char*>(&current), sizeof(Value))) {
            if (current < prev) {
                return false;
            }
            prev = current;
        }

        return true;
    }

}  // namespace bigsort