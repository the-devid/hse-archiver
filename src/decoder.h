#pragma once

#include "huffman_encoder.h"
#include "bit_reader.h"
#include "bit_writer.h"

namespace archiver {
class Decoder {
public:
    void DecodeArchive(BitReader& reader);
    bool DecodeFile(BitReader& reader);
    void DecodeCanonicalCodes(BitReader& reader);
    ExtChar DecodeUntilSpecialChar(BitReader& reader, BitWriter& writer);

private:
    HuffmanEncoder coder_;
    BinaryTrie huffman_codes_;
};
}  // namespace archiver
