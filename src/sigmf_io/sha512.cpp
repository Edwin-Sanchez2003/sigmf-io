#include "sigmf_io/sha512.h"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <stdexcept>

namespace sigmf_io {

SHA512::SHA512(const std::string& hex)
    : raw_(hex)
{
    parse_and_validate(hex);
}

void SHA512::parse_and_validate(const std::string& hex)
{
    if (hex.size() != k_hex_length) {
        throw std::invalid_argument(
            "SHA512: '" + hex + "' is not " + std::to_string(k_hex_length) +
            " hex characters (got " + std::to_string(hex.size()) + ")");
    }

    normalized_.resize(k_hex_length);
    for (std::size_t i = 0; i < k_hex_length; ++i) {
        const unsigned char c = static_cast<unsigned char>(hex[i]);
        if (!std::isxdigit(c)) {
            throw std::invalid_argument(
                "SHA512: '" + hex + "' contains a non-hex character at position " +
                std::to_string(i));
        }
        normalized_[i] = static_cast<char>(std::tolower(c));
    }

    for (std::size_t byte_i = 0; byte_i < k_digest_bytes; ++byte_i) {
        unsigned int value = 0;
        const char* start = normalized_.data() + byte_i * 2;
        std::from_chars(start, start + 2, value, 16);
        bytes_[byte_i] = static_cast<std::byte>(value);
    }
}

bool SHA512::matches(std::span<const std::byte, k_digest_bytes> digest) const
{
    return std::equal(bytes_.begin(), bytes_.end(), digest.begin());
}

bool SHA512::matches(std::span<const unsigned char, k_digest_bytes> digest) const
{
    return std::equal(as_bytes_view().begin(), as_bytes_view().end(), digest.begin());
}

} // end sigmf_io namespace
