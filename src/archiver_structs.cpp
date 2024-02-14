#include "archiver_structs.h"
#include "archiver_exceptions.h"
#include <stdexcept>

size_t archiver::HuffmanCode::Size() const {
    return code_.size();
}
bool archiver::HuffmanCode::At(size_t pos) const {
    if (pos >= Size()) {
        throw std::out_of_range("Wrong index when looking at Huffman code");
    }
    return code_[pos];
}
archiver::HuffmanCode::HuffmanCode(size_t len) : code_(len) {
}
archiver::HuffmanCode::HuffmanCode(const std::vector<bool>& code) : code_(code) {
}
archiver::HuffmanCode archiver::HuffmanCode::GenNextCanonicalHuffmanCode(size_t next_len) const {
    if (code_.empty()) {
        throw archiver::EmptyCodeException("Somehow get empty Huffman\'s code");
    }
    std::vector<bool> result = code_;
    size_t ind = result.size();
    while (ind > 0 && result[ind - 1]) {
        --ind;
    }
    if (ind == 0) {
        result.insert(result.begin(), false);
        ++ind;
    }
    result[ind - 1] = true;
    for (size_t after_one_ind = ind; after_one_ind < result.size(); ++after_one_ind) {
        result[after_one_ind] = false;
    }
    result.resize(next_len);
    return HuffmanCode(result);
}
bool archiver::HuffmanCode::operator==(const archiver::HuffmanCode& oth) const {
    return code_ == oth.code_;
}
bool archiver::CharNCode::operator==(const archiver::CharNCode& oth) const {
    return chr == oth.chr && code == oth.code;
}
