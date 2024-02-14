#pragma once

#include "archiver_constants.h"
#include "archiver_structs.h"
#include "binary_trie.h"
#include <unordered_map>

namespace archiver {
class HuffmanEncoder {
public:
    HuffmanEncoder() = default;
    void AddCustomCharFrequency(ExtChar chr, size_t chr_freq = 1);
    void BuildCodeLengthsFromFrequencies();
    void CanonizeCodes();
    std::vector<CharNCode> GetCanonicalCodes() const;
    HuffmanCode GetCode(ExtChar chr) const;
    void Reset();
    void StartFromLens(std::unordered_map<ExtChar, size_t> huffman_lens);
    BinaryTrie BuildTrieByCanonicalCodes();

private:
    std::unordered_map<ExtChar, size_t> frequencies_;
    std::unordered_map<ExtChar, size_t> huffman_lens_;
    std::unordered_map<ExtChar, HuffmanCode> huffman_codes_;
    std::vector<CharNCode> canonical_codes_;
    BinaryTrie codes_trie_;
};
}  // namespace archiver
