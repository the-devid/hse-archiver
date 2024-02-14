#include "bit_reader.h"
#include "archiver_constants.h"
#include "archiver_exceptions.h"

namespace archiver {
BitReader::BitReader(std::istream& in) : in_(&in) {
}
void BitReader::ReadInBuffer() {
    std::fill(buffer_.get(), buffer_.get() + BUFFER_CAPACITY, 0);
    valid_bits_ = 0;
    in_->read(buffer_.get(), static_cast<std::streamsize>(BUFFER_CAPACITY));
    valid_bits_ = in_->gcount();
    current_bit_ind_ = 0;
}
int BitReader::GetBit() {
    if (bad_stream_) {
        throw BitReadException("Trying to read from ended stream");
    }
    if (current_bit_ind_ >= valid_bits_ * BITS_IN_CHAR) {
        ReadInBuffer();
    }
    if (current_bit_ind_ >= valid_bits_ * BITS_IN_CHAR) {
        bad_stream_ = true;
        return BAD_BITS;
    }
    int result = buffer_[current_bit_ind_ / BITS_IN_CHAR] >> ((BITS_IN_CHAR - 1 - current_bit_ind_ % BITS_IN_CHAR)) &
                 1;  // getting needed bit
    ++current_bit_ind_;
    return result;
}
ExtChar BitReader::GetBits(size_t number_of_bits) {
    ExtChar result = 0;
    for (size_t bit_iter = 0; bit_iter < number_of_bits; ++bit_iter) {
        result <<= 1;
        if (bad_stream_) {
            return BAD_BITS;
        }
        result += GetBit();
    }
    return result;
}
void BitReader::ResetIstream(std::istream& in) {
    in_ = &in;
    valid_bits_ = 0;
    current_bit_ind_ = 0;
    bad_stream_ = false;
}
}  // namespace archiver
