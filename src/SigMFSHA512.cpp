#include "SigMFSHA512.h"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <stdexcept>

SigMFSHA512::SigMFSHA512(const std::string& hex)
    : raw_(hex)
{
    parse_and_validate(hex);
}

void SigMFSHA512::parse_and_validate(const std::string& hex)
{
    if (hex.size() != kHexLength) {
        throw std::invalid_argument(
            "SigMFSHA512: '" + hex + "' is not " + std::to_string(kHexLength) +
            " hex characters (got " + std::to_string(hex.size()) + ")");
    }

    normalized_.resize(kHexLength);
    for (std::size_t i = 0; i < kHexLength; ++i) {
        const unsigned char c = static_cast<unsigned char>(hex[i]);
        if (!std::isxdigit(c)) {
            throw std::invalid_argument(
                "SigMFSHA512: '" + hex + "' contains a non-hex character at position " +
                std::to_string(i));
        }
        normalized_[i] = static_cast<char>(std::tolower(c));
    }

    for (std::size_t byte_i = 0; byte_i < kDigestBytes; ++byte_i) {
        unsigned int value = 0;
        const char* start = normalized_.data() + byte_i * 2;
        std::from_chars(start, start + 2, value, 16);
        bytes_[byte_i] = static_cast<std::byte>(value);
    }
}

bool SigMFSHA512::matches(std::span<const std::byte, kDigestBytes> digest) const
{
    return std::equal(bytes_.begin(), bytes_.end(), digest.begin());
}

bool SigMFSHA512::matches(std::span<const unsigned char, kDigestBytes> digest) const
{
    return std::equal(as_bytes_view().begin(), as_bytes_view().end(), digest.begin());
}
