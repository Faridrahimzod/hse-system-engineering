#include "file_mapping.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>
#include <utility>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace bigsort {

    namespace {

        std::size_t PageSize() {
            long value = ::sysconf(_SC_PAGESIZE);
            if (value <= 0) {
                throw std::runtime_error("Failed to get page size");
            }
            return static_cast<std::size_t>(value);
        }

    }  // namespace

    ReadOnlyFileMapping::ReadOnlyFileMapping(
        const std::string& path,
        std::size_t offset_bytes,
        std::size_t length_bytes) {
        fd_ = ::open(path.c_str(), O_RDONLY);
        if (fd_ < 0) {
            throw std::system_error(errno, std::generic_category(), "open failed");
        }

        if (length_bytes == 0) {
            length_ = 0;
            return;
        }

        const std::size_t page_size = PageSize();
        const std::size_t aligned_offset = (offset_bytes / page_size) * page_size;
        delta_ = offset_bytes - aligned_offset;
        base_mapping_size_ = delta_ + length_bytes;
        length_ = length_bytes;

        base_mapping_ = ::mmap(
            nullptr,
            base_mapping_size_,
            PROT_READ,
            MAP_PRIVATE,
            fd_,
            static_cast<off_t>(aligned_offset));

        if (base_mapping_ == MAP_FAILED) {
            base_mapping_ = nullptr;
            ::close(fd_);
            fd_ = -1;
            throw std::system_error(errno, std::generic_category(), "mmap failed");
        }
    }

    ReadOnlyFileMapping::~ReadOnlyFileMapping() {
        Close();
    }

    ReadOnlyFileMapping::ReadOnlyFileMapping(ReadOnlyFileMapping&& other) noexcept
        : fd_(other.fd_),
        base_mapping_(other.base_mapping_),
        base_mapping_size_(other.base_mapping_size_),
        delta_(other.delta_),
        length_(other.length_) {
        other.fd_ = -1;
        other.base_mapping_ = nullptr;
        other.base_mapping_size_ = 0;
        other.delta_ = 0;
        other.length_ = 0;
    }

    ReadOnlyFileMapping& ReadOnlyFileMapping::operator=(ReadOnlyFileMapping&& other) noexcept {
        if (this != &other) {
            Close();
            fd_ = other.fd_;
            base_mapping_ = other.base_mapping_;
            base_mapping_size_ = other.base_mapping_size_;
            delta_ = other.delta_;
            length_ = other.length_;

            other.fd_ = -1;
            other.base_mapping_ = nullptr;
            other.base_mapping_size_ = 0;
            other.delta_ = 0;
            other.length_ = 0;
        }
        return *this;
    }

    const std::byte* ReadOnlyFileMapping::Data() const {
        if (length_ == 0 || base_mapping_ == nullptr) {
            return nullptr;
        }
        return reinterpret_cast<const std::byte*>(base_mapping_) + delta_;
    }

    std::size_t ReadOnlyFileMapping::Size() const {
        return length_;
    }

    void ReadOnlyFileMapping::Close() {
        if (base_mapping_ != nullptr) {
            ::munmap(base_mapping_, base_mapping_size_);
            base_mapping_ = nullptr;
        }
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
        base_mapping_size_ = 0;
        delta_ = 0;
        length_ = 0;
    }

}  // namespace bigsort