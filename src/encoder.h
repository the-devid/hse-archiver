#pragma once

#include <unordered_map>
#include <deque>
#include "archiver_structs.h"
#include "bit_reader.h"
#include "binary_trie.h"
#include "bit_writer.h"
#include "huffman_encoder.h"

namespace archiver {
class Encoder {
public:
    void CalculateFrequencies(BitReader& reader);
    void InitCodes();
    void EncodeAndPutChar(ExtChar chr, BitWriter& writer);
    void EncodeAndWrite(BitReader& reader, BitWriter& writer);
    void EncodeFile(std::string path_to_file, BitWriter& writer, bool is_last_file = false);
    void Reset();

private:
    HuffmanEncoder coder_;
};
}  // namespace archiver