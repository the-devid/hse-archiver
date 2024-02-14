#pragma once

#include <cstddef>
namespace archiver {
constexpr size_t BITS_IN_BYTE = 8;                            // NOLINT
constexpr size_t BITS_IN_CHAR = sizeof(char) * BITS_IN_BYTE;  // NOLINT
using ExtChar = int;
constexpr ExtChar FILENAME_END = 256;   // NOLINT
constexpr ExtChar ONE_MORE_FILE = 257;  // NOLINT
constexpr ExtChar ARCHIVE_END = 258;    // NOLINT
constexpr size_t BITS_IN_EXTCHAR = 9;   // NOLINT
constexpr int MAIN_BAD_RETURN = 111;    // NOLINT
}  // namespace archiver
