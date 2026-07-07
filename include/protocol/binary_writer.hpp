#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace mfe {

class BinaryWriter {
public:
    explicit BinaryWriter(std::vector<std::uint8_t>& buffer)
        : buffer_(buffer) {}

    template<typename T>
    void write(const T& value)
    {
        const auto* ptr =
            reinterpret_cast<const std::uint8_t*>(&value);

        buffer_.insert(buffer_.end(), ptr, ptr + sizeof(T));
    }

    void write_bytes(const char* data, std::size_t size)
    {
        buffer_.insert(buffer_.end(), data, data + size);
    }

private:
    std::vector<std::uint8_t>& buffer_;
};

}