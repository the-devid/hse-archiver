#include <catch.hpp>

#include "bit_reader.h"
#include "bit_writer.h"
#include "encoder.h"
#include "huffman_encoder.h"
#include "min_heap.h"

#include <queue>
#include <sstream>

TEST_CASE("BitReader") {
  std::vector<char> strchrv = {0b01110010, 0b00001000};
  std::string str = "0111001000001000";
  std::string strchr;
  for (auto &chr : strchrv) {
    strchr += chr;
  }
  std::stringstream strin(strchr);
  archiver::BitReader reader(strin);
  for (size_t ind = 0; ind < str.size(); ++ind) {
    REQUIRE(static_cast<int>(str[ind] - '0') == reader.GetBit());
  }
  strin = std::stringstream(strchr);
  reader.ResetIstream(strin);
  for (size_t ind = 0; ind < str.size(); ++ind) {
    REQUIRE(static_cast<int>(str[ind] - '0') == reader.GetBit());
  }
}

TEST_CASE("BitWriter") {
  std::string str1 = "S$udflijv;409DFG\\s\r\t\n";
  std::stringstream strout;
  archiver::BitWriter writer(strout);
  for (auto &chr : str1) {
    for (int bit = 7; bit >= 0; --bit) {
      writer.WriteBit(static_cast<char>(chr >> bit & 1));
    }
  }
  writer.Flush();
  REQUIRE(strout.str() == str1);
}

TEST_CASE("BitstreamsCheckerSmall") {
  std::vector<unsigned char> bytes = {12, 34,  2,   3,   56,  122, 34, 0,
                                      23, 165, 172, 134, 154, 166, 124};
  std::stringstream stroutin;
  archiver::BitWriter bitout(stroutin);
  for (const auto &x : bytes) {
    bitout.WriteBits(archiver::BITS_IN_CHAR, x);
  }
  bitout.Flush();
  archiver::BitReader bitin(stroutin);
  std::vector<unsigned char> result;
  archiver::ExtChar x = bitin.GetBits(archiver::BITS_IN_CHAR);
  while (x != archiver::BitReader::BAD_BITS) {
    result.emplace_back(x);
    x = bitin.GetBits(archiver::BITS_IN_CHAR);
  }
  REQUIRE(result == bytes);
}

TEST_CASE("BitstreamsCheckerLargeRandom") {
  srand(5);
  static const size_t N = 50000;
  std::vector<unsigned char> bytes(N);
  for (auto &x : bytes) {
    x = rand() % (1 << archiver::BITS_IN_CHAR);
  }
  std::stringstream stroutin;
  archiver::BitWriter bitout(stroutin);
  for (const auto &x : bytes) {
    bitout.WriteBits(archiver::BITS_IN_CHAR, x);
  }
  bitout.Flush();
  archiver::BitReader bitin(stroutin);
  std::vector<unsigned char> result;
  archiver::ExtChar x = bitin.GetBits(archiver::BITS_IN_CHAR);
  while (x != archiver::BitReader::BAD_BITS) {
    result.emplace_back(x);
    x = bitin.GetBits(archiver::BITS_IN_CHAR);
  }
  REQUIRE(result == bytes);
}

TEST_CASE("HuffmanCanonicalCoder") {
  archiver::HuffmanEncoder huff_coder;
  huff_coder.AddCustomCharFrequency('A', 2);
  huff_coder.AddCustomCharFrequency('B', 4);
  huff_coder.AddCustomCharFrequency('C', 1);
  huff_coder.AddCustomCharFrequency('D', 1);
  huff_coder.BuildCodeLengthsFromFrequencies();
  huff_coder.CanonizeCodes();
  std::vector<archiver::CharNCode> expected_code = {
      archiver::CharNCode('B', archiver::HuffmanCode(std::vector<bool>{false})),
      archiver::CharNCode(
          'A', archiver::HuffmanCode(std::vector<bool>{true, false})),
      archiver::CharNCode(
          'C', archiver::HuffmanCode(std::vector<bool>{true, true, false})),
      archiver::CharNCode(
          'D', archiver::HuffmanCode(std::vector<bool>{true, true, true}))};
  REQUIRE(huff_coder.GetCanonicalCodes() == expected_code);
  std::string str1 = "CABBDABB";
  std::stringstream strin(str1);
  archiver::BitReader bitin(strin);
  archiver::Encoder encoder;
  encoder.CalculateFrequencies(bitin);
  encoder.InitCodes();
  strin.clear();
  strin.str(str1);
  bitin.ResetIstream(strin);
  std::stringstream strout;
  archiver::BitWriter bitout(strout);
  std::vector<unsigned char> expected_chrs = {0b11010001, 0b11100000};
  std::string expected_str;
  for (auto chr : expected_chrs) {
    expected_str += static_cast<char>(chr);
  }
  encoder.EncodeAndWrite(bitin, bitout);
  bitout.Flush();
  REQUIRE(expected_str == strout.str());
}

TEST_CASE("HuffmanCanonicalCoderEqualLens") {
  static const size_t EXP_LEN = 10;
  static const archiver::ExtChar MIN_CHR = 0;
  static const archiver::ExtChar MAX_CHR = (1 << EXP_LEN) - 1;
  archiver::HuffmanEncoder huff_coder;
  std::vector<archiver::ExtChar> chrlist;
  for (archiver::ExtChar c = MIN_CHR; c <= MAX_CHR; ++c) {
    chrlist.emplace_back(c);
    huff_coder.AddCustomCharFrequency(c, 1);
  }
  huff_coder.BuildCodeLengthsFromFrequencies();
  huff_coder.CanonizeCodes();
  size_t chr_ind = 0;
  for (auto &[chr, code] : huff_coder.GetCanonicalCodes()) {
    REQUIRE(chr == static_cast<archiver::ExtChar>(MIN_CHR + chr_ind));
    std::vector<bool> chr_ind_bits(EXP_LEN);
    for (size_t bit_ind = 0; bit_ind < EXP_LEN; ++bit_ind) {
      REQUIRE(code.At(EXP_LEN - 1 - bit_ind) ==
              static_cast<bool>((chr_ind >> bit_ind) & 1));
    }
    ++chr_ind;
  }
}

TEST_CASE("HeapRandomSort") {
  srand(5);
  archiver::MinHeap<int> myheap;
  std::priority_queue<int, std::vector<int>, std::greater<>> stdheap;
  static const size_t ITERS = 100000;
  for (size_t iter = 0; iter < ITERS; ++iter) {
    int x = rand();
    myheap.Push(x);
    stdheap.push(x);
  }
  for (size_t iter = 0; iter < ITERS; ++iter) {
    REQUIRE(myheap.Top() == stdheap.top());
    myheap.Pop();
    stdheap.pop();
  }
}
