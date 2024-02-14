#include <queue>
#include <sstream>
#include <fstream>
#include "encoder.h"
#include "archiver_constants.h"

void archiver::Encoder::CalculateFrequencies(archiver::BitReader& reader) {
    ExtChar chr = reader.GetBits(BITS_IN_CHAR);
    while (chr != BitReader::BAD_BITS) {
        coder_.AddCustomCharFrequency(chr);
        chr = reader.GetBits(BITS_IN_CHAR);
    }
}
void archiver::Encoder::EncodeAndWrite(archiver::BitReader& reader, archiver::BitWriter& writer) {
    ExtChar chr = reader.GetBits(BITS_IN_CHAR);
    while (chr != BitReader::BAD_BITS) {
        EncodeAndPutChar(chr, writer);
        chr = reader.GetBits(BITS_IN_CHAR);
    }
}
void archiver::Encoder::InitCodes() {
    coder_.BuildCodeLengthsFromFrequencies();
    coder_.CanonizeCodes();
}
void archiver::Encoder::EncodeFile(std::string path_to_file, archiver::BitWriter& writer, bool is_last_file) {
    std::string filename;
    size_t non_path_ind = path_to_file.size();
    while (non_path_ind > 0 && path_to_file[non_path_ind - 1] != '/') {
        --non_path_ind;
    }
    filename = path_to_file.substr(non_path_ind, path_to_file.size() - non_path_ind);
    std::stringstream filename_stream(filename);
    BitReader filename_reader(filename_stream);
    CalculateFrequencies(filename_reader);
    std::ifstream file_stream(path_to_file);
    BitReader file_reader(file_stream);
    CalculateFrequencies(file_reader);
    coder_.AddCustomCharFrequency(archiver::FILENAME_END);
    coder_.AddCustomCharFrequency(archiver::ONE_MORE_FILE);
    coder_.AddCustomCharFrequency(archiver::ARCHIVE_END);
    InitCodes();
    auto canonical_codes = coder_.GetCanonicalCodes();
    writer.WriteBits(archiver::BITS_IN_EXTCHAR, canonical_codes.size());
    size_t max_code_len = 0;
    for (auto& [chr, code] : canonical_codes) {
        writer.WriteBits(archiver::BITS_IN_EXTCHAR, chr);
        max_code_len = std::max(max_code_len, code.Size());
    }
    std::vector<ExtChar> how_many_chrs_with_len(max_code_len);
    for (auto& [chr, code] : canonical_codes) {
        ++how_many_chrs_with_len[code.Size() - 1];
    }
    for (const auto& amount_of_chars : how_many_chrs_with_len) {
        writer.WriteBits(BITS_IN_EXTCHAR, amount_of_chars);
    }
    filename_stream.clear();
    filename_stream.str(filename);
    file_stream.clear();
    file_stream.close();
    file_stream.open(path_to_file);
    filename_reader.ResetIstream(filename_stream);
    file_reader.ResetIstream(file_stream);
    EncodeAndWrite(filename_reader, writer);
    EncodeAndPutChar(FILENAME_END, writer);
    EncodeAndWrite(file_reader, writer);
    if (is_last_file) {
        EncodeAndPutChar(ARCHIVE_END, writer);
    } else {
        EncodeAndPutChar(ONE_MORE_FILE, writer);
    }
    Reset();
}
void archiver::Encoder::EncodeAndPutChar(archiver::ExtChar chr, archiver::BitWriter& writer) {
    auto result = coder_.GetCode(chr);
    for (size_t ind = 0; ind < result.Size(); ++ind) {
        writer.WriteBit(static_cast<char>(result.At(ind)));
    }
}
void archiver::Encoder::Reset() {
    coder_.Reset();
}
