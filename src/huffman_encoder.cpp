#include "huffman_encoder.h"
#include "min_heap.h"
#include <stdexcept>

void archiver::HuffmanEncoder::AddCustomCharFrequency(ExtChar chr, size_t chr_freq) {
    frequencies_[chr] += chr_freq;
}
void archiver::HuffmanEncoder::BuildCodeLengthsFromFrequencies() {
    struct PrioritisedChar {
        ExtChar chr;
        size_t freq;
        BinaryTrie trie;
        bool operator<(const PrioritisedChar& other) const {
            if (freq != other.freq) {
                return freq < other.freq;
            }
            return chr < other.chr;
        }
        bool operator>(const PrioritisedChar& other) const {
            return other < *this;
        }
    };
    archiver::MinHeap<PrioritisedChar> prior_queue_of_chars;  // pairs (char, frequency)
    std::unordered_map<ExtChar, BinaryTrie> chars_vertices;
    std::unordered_map<ExtChar, BinaryTrie::Iterator> chars_iterators;
    for (const auto& [chr, freq] : frequencies_) {
        chars_vertices[chr] = BinaryTrie();
        chars_iterators[chr] = chars_vertices[chr].Root();
        prior_queue_of_chars.Push(PrioritisedChar{.chr = chr, .freq = freq, .trie = chars_vertices[chr]});
    }
    while (prior_queue_of_chars.Size() > 1) {
        auto first_least_freq_chr = prior_queue_of_chars.Top();
        prior_queue_of_chars.Pop();
        auto second_least_freq_chr = prior_queue_of_chars.Top();
        prior_queue_of_chars.Pop();
        prior_queue_of_chars.Push(
            PrioritisedChar{.chr = std::min(first_least_freq_chr.chr, second_least_freq_chr.chr),
                            .freq = first_least_freq_chr.freq + second_least_freq_chr.freq,
                            .trie = BinaryTrie::NewInternal(first_least_freq_chr.trie, second_least_freq_chr.trie)});
    }
    codes_trie_ = prior_queue_of_chars.Top().trie;
    for (auto [chr, chr_iter] : chars_iterators) {
        huffman_lens_[chr] = codes_trie_.LengthOfPathFromRoot(chr_iter);
    }
}
void archiver::HuffmanEncoder::CanonizeCodes() {
    struct CharAndHLen {
        ExtChar chr;
        size_t code_len;
    };
    std::vector<CharAndHLen> chars_and_lens;
    for (const auto& [chr, len] : huffman_lens_) {
        chars_and_lens.emplace_back(CharAndHLen{.chr = chr, .code_len = len});
    }
    std::sort(chars_and_lens.begin(), chars_and_lens.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.code_len != rhs.code_len) {
            return lhs.code_len < rhs.code_len;
        }
        return lhs.chr < rhs.chr;
    });
    if (chars_and_lens.empty()) {
        return;
    }
    canonical_codes_.emplace_back(chars_and_lens[0].chr, HuffmanCode(chars_and_lens[0].code_len));
    for (size_t chr_ind = 1; chr_ind < chars_and_lens.size(); ++chr_ind) {
        canonical_codes_.emplace_back(
            chars_and_lens[chr_ind].chr,
            canonical_codes_[chr_ind - 1].code.GenNextCanonicalHuffmanCode(chars_and_lens[chr_ind].code_len));
    }
    for (auto& [chr, code] : canonical_codes_) {
        huffman_codes_[chr] = code;
    }
}
std::vector<archiver::CharNCode> archiver::HuffmanEncoder::GetCanonicalCodes() const {
    return canonical_codes_;
}
archiver::HuffmanCode archiver::HuffmanEncoder::GetCode(archiver::ExtChar chr) const {
    if (!huffman_codes_.contains(chr)) {
        throw std::out_of_range(
            "Unknown char in file (Huffman coder's error). Probably changed during archiving, or fatal error");
    }
    return huffman_codes_.at(chr);
}
void archiver::HuffmanEncoder::Reset() {
    frequencies_ = {};
    huffman_lens_ = {};
    huffman_codes_ = {};
    canonical_codes_ = {};
    codes_trie_ = BinaryTrie();
}
void archiver::HuffmanEncoder::StartFromLens(std::unordered_map<ExtChar, size_t> huffman_lens) {
    Reset();
    huffman_lens_ = huffman_lens;
}
archiver::BinaryTrie archiver::HuffmanEncoder::BuildTrieByCanonicalCodes() {
    BinaryTrie result;
    for (const auto& [chr, code] : canonical_codes_) {
        auto cur_iter = result.Root();
        for (size_t bit_ind = 0; bit_ind < code.Size(); ++bit_ind) {
            result.AddChild(cur_iter, code.At(bit_ind));
            cur_iter = cur_iter.Next(code.At(bit_ind));
        }
        result.SetTerminalChar(cur_iter, chr);
    }
    return result;
}
