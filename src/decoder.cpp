#include <sstream>
#include <fstream>
#include "decoder.h"
#include "archiver_constants.h"
#include "archiver_exceptions.h"

void archiver::Decoder::DecodeArchive(archiver::BitReader& reader) {
    bool need_to_continue = false;
    do {
        need_to_continue = DecodeFile(reader);
    } while (need_to_continue);
}
bool archiver::Decoder::DecodeFile(archiver::BitReader& reader) {
    DecodeCanonicalCodes(reader);
    std::stringstream filename_stream;
    BitWriter filename_bitstream(filename_stream);
    auto chr = DecodeUntilSpecialChar(reader, filename_bitstream);
    filename_bitstream.Flush();
    if (chr != archiver::FILENAME_END) {
        throw archiver::WrongArchiveException("FILENAME_END expected, but read something other");
    }
    std::string filename = filename_stream.str();
    std::ofstream file_stream(filename);
    BitWriter file_bitstream(file_stream);
    chr = DecodeUntilSpecialChar(reader, file_bitstream);
    if (chr != archiver::ONE_MORE_FILE && chr != archiver::ARCHIVE_END) {
        throw archiver::WrongArchiveException("End of file expected, but read something other");
    }
    return chr == archiver::ONE_MORE_FILE;
}
void archiver::Decoder::DecodeCanonicalCodes(archiver::BitReader& reader) {
    size_t symbols_count = reader.GetBits(BITS_IN_EXTCHAR);
    std::vector<ExtChar> chars;
    for (size_t chr_ind = 0; chr_ind < symbols_count; ++chr_ind) {
        chars.emplace_back(reader.GetBits(BITS_IN_EXTCHAR));
    }
    std::unordered_map<ExtChar, size_t> huffman_lens;
    size_t chr_ptr = 0;
    size_t len_ptr = 1;
    size_t summary_amount_of_chars = 0;
    while (summary_amount_of_chars != symbols_count) {
        size_t chars_with_cur_len = reader.GetBits(BITS_IN_EXTCHAR);
        for (size_t putting_lens_iters = 0; putting_lens_iters < chars_with_cur_len; ++putting_lens_iters) {
            huffman_lens[chars[chr_ptr]] = len_ptr;
            ++chr_ptr;
        }
        ++len_ptr;
        summary_amount_of_chars += chars_with_cur_len;
    }
    coder_.StartFromLens(huffman_lens);
    coder_.CanonizeCodes();
    huffman_codes_ = coder_.BuildTrieByCanonicalCodes();
}
archiver::ExtChar archiver::Decoder::DecodeUntilSpecialChar(archiver::BitReader& reader, archiver::BitWriter& writer) {
    auto cur_iter = huffman_codes_.Root();
    while (true) {  // ends when reader is ends (at least)
        auto yet_another_bit = reader.GetBit();
        cur_iter = cur_iter.Next(yet_another_bit);
        if (cur_iter.IsNull()) {
            throw archiver::UnknownCharException("When decoding, the unknown character in archive met");
        }
        if (cur_iter.Chr() != -1) {
            if (cur_iter.Chr() == archiver::FILENAME_END || cur_iter.Chr() == archiver::ONE_MORE_FILE ||
                cur_iter.Chr() == archiver::ARCHIVE_END) {
                return cur_iter.Chr();
            }
            writer.WriteBits(BITS_IN_CHAR, cur_iter.Chr());
            cur_iter = huffman_codes_.Root();
        }
    }
}
