#pragma once

#include <cstddef>
#include <iosfwd>
#include <iostream>
#include <memory>

namespace archiver {
class BitWriter {
public:
    BitWriter() = default;
    explicit BitWriter(std::ostream& out);
    void WriteBit(char bit);
    void WriteBits(size_t n, uint64_t bits);
    void Flush();
    ~BitWriter();

private:
    static const size_t BUFFER_CAPACITY = 512;  // size of buffer in chars
    std::unique_ptr<char[]> buffer_ = std::make_unique<char[]>(BUFFER_CAPACITY);
    size_t current_bit_ind_ = 0;
    std::ostream& out_ = std::cout;
    void WriteInBuffer();
};
}  // namespace archiver
