#pragma once

#include <cstddef>
#include <vector>
#include "archiver_constants.h"

namespace archiver {

class HuffmanCode {
public:
    HuffmanCode() = default;
    explicit HuffmanCode(size_t len);
    explicit HuffmanCode(const std::vector<bool>& code);
    size_t Size() const;
    bool At(size_t pos) const;
    HuffmanCode GenNextCanonicalHuffmanCode(size_t diff_len) const;
    bool operator==(const HuffmanCode& oth) const;

private:
    std::vector<bool> code_;
};
struct CharNCode {
    ExtChar chr;
    HuffmanCode code;
    CharNCode(const ExtChar& chr, const HuffmanCode& code) : chr(chr), code(code) {
    }
    bool operator==(const CharNCode& oth) const;
};
}  // namespace archiver