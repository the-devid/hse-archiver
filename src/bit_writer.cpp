#include "bit_writer.h"
#include "archiver_constants.h"

archiver::BitWriter::BitWriter(std::ostream& out) : out_(out) {
    std::fill(buffer_.get(), buffer_.get() + BUFFER_CAPACITY, 0);
}
void archiver::BitWriter::WriteInBuffer() {
    out_.write(buffer_.get(), static_cast<std::streamsize>((current_bit_ind_ + BITS_IN_CHAR - 1) / BITS_IN_CHAR));
    std::fill(buffer_.get(), buffer_.get() + BUFFER_CAPACITY, 0);
    current_bit_ind_ = 0;
}
void archiver::BitWriter::WriteBit(char bit) {
    if (current_bit_ind_ == BUFFER_CAPACITY * BITS_IN_CHAR) {
        WriteInBuffer();
    }
    buffer_[current_bit_ind_ / BITS_IN_CHAR] = static_cast<char>(
        buffer_[current_bit_ind_ / BITS_IN_CHAR] | (bit << (BITS_IN_CHAR - 1 - (current_bit_ind_ % BITS_IN_CHAR))));
    ++current_bit_ind_;
}
void archiver::BitWriter::Flush() {
    WriteInBuffer();
}
void archiver::BitWriter::WriteBits(size_t n, uint64_t bits) {
    for (size_t ind = static_cast<size_t>(n); ind >= 1; --ind) {
        WriteBit(static_cast<char>((bits >> (ind - 1)) & 1));
    }
}
archiver::BitWriter::~BitWriter() {
    WriteInBuffer();
}
