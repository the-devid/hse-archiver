#pragma once

#include <cstddef>
#include <iosfwd>
#include <iostream>
#include <memory>
#include "archiver_constants.h"

namespace archiver {
class BitReader {
public:
    static constexpr int BAD_BITS = -1;  // NOLINT
    BitReader() = default;
    explicit BitReader(std::istream& in);
    int GetBit();
    ExtChar GetBits(size_t number_of_bits);
    void ResetIstream(std::istream& in);

private:
    static const size_t BUFFER_CAPACITY = 512;  // size of buffer in chars
    size_t valid_bits_ = 0;
    std::unique_ptr<char[]> buffer_ = std::make_unique<char[]>(BUFFER_CAPACITY);
    size_t current_bit_ind_ = 0;
    std::istream* in_ = &std::cin;
    bool bad_stream_ = false;
    void ReadInBuffer();
};
}  // namespace archiver