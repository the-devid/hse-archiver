#pragma once

#include <stdexcept>

namespace archiver {
class BitReadException : public std::runtime_error {
public:
    explicit BitReadException(const std::string& what) : std::runtime_error(what) {
    }
};
class UnknownCharException : public std::runtime_error {
public:
    explicit UnknownCharException(const std::string& what) : std::runtime_error(what) {
    }
};
class WrongArchiveException : public std::runtime_error {
public:
    explicit WrongArchiveException(const std::string& what) : std::runtime_error(what) {
    }
};
class EmptyCodeException : public std::runtime_error {
public:
    explicit EmptyCodeException(const std::string& what) : std::runtime_error(what) {
    }
};
}  // namespace archiver
