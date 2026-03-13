#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace bigsort {

	class ReadOnlyFileMapping {
	public:
		ReadOnlyFileMapping(const std::string& path, std::size_t offset_bytes, std::size_t length_bytes);
		~ReadOnlyFileMapping();

		ReadOnlyFileMapping(const ReadOnlyFileMapping&) = delete;
		ReadOnlyFileMapping& operator=(const ReadOnlyFileMapping&) = delete;

		ReadOnlyFileMapping(ReadOnlyFileMapping&& other) noexcept;
		ReadOnlyFileMapping& operator=(ReadOnlyFileMapping&& other) noexcept;

		const std::byte* Data() const;
		std::size_t Size() const;

	private:
		void Close();

	private:
		int fd_{ -1 };
		void* base_mapping_{ nullptr };
		std::size_t base_mapping_size_{ 0 };
		std::size_t delta_{ 0 };
		std::size_t length_{ 0 };
	};

}  // namespace bigsort