#pragma once
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
#include <type_traits>

namespace mfe {
class BinaryWriter {
public:
    explicit BinaryWriter(std::span<std::uint8_t> buffer)
        : buffer_(buffer), offset_(0) {}

    template<typename T>
    requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
    void write(const T& value)
    {
        if (offset_ + sizeof(T) > buffer_.size()) {
            throw std::runtime_error("Buffer overflow");
        }
        std::memcpy(buffer_.data() + offset_, &value, sizeof(T));
        offset_ += sizeof(T);
    }
    void write_bytes(const char* data, std::size_t size)
    {
        if (offset_ + size > buffer_.size()) {
            throw std::runtime_error("Buffer overflow");
        }
        std::memcpy(buffer_.data() + offset_, data, size);
        offset_ += size;
    }
    std::size_t bytes_written() const { return offset_; }
private:
    std::span<std::uint8_t> buffer_;
    std::size_t offset_;
};
}