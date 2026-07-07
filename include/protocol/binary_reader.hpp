#pragma once

#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
#include <type_traits>

namespace mfe {
class BinaryReader {
public:
    explicit BinaryReader(std::span<const std::uint8_t> buffer)
        : buffer_(buffer), offset_(0) {}
    template<typename T>
    requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
    T read()
    {
        if (offset_ + sizeof(T) > buffer_.size()) {
            throw std::runtime_error("Buffer underflow");
        }
        T value{};
        std::memcpy(&value, buffer_.data() + offset_, sizeof(T));
        offset_ += sizeof(T);
        return value;
    }
    void read_bytes(char* dst, std::size_t size)
    {
        if (offset_ + size > buffer_.size()) {
            throw std::runtime_error("Buffer underflow");
        }
        std::memcpy(dst, buffer_.data() + offset_, size);
        offset_ += size;
    }
private:
    std::span<const std::uint8_t> buffer_;
    std::size_t offset_;
};
}