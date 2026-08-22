#ifndef SIGMF_IO_SHA512_H
#define SIGMF_IO_SHA512_H
#include <array>
#include <compare>
#include <cstddef>
#include <span>
#include <string>

namespace sigmf_io {

class SHA512 {
public:
    static constexpr std::size_t kDigestBytes = 64;
    static constexpr std::size_t kHexLength   = kDigestBytes * 2;

    explicit SHA512(const std::string& hex);

    std::string to_string() const { return raw_; }
    std::string hex() const { return normalized_; }
    std::array<std::byte, kDigestBytes> bytes() const { return bytes_; }

    // Zero-cost view for interop with C-style crypto APIs (OpenSSL, etc.)
    // that write digests as unsigned char buffers rather than std::byte.
    std::span<const unsigned char, kDigestBytes> as_bytes_view() const {
        return std::span<const unsigned char, kDigestBytes>(
            reinterpret_cast<const unsigned char*>(bytes_.data()), kDigestBytes);
    }

    std::strong_ordering operator<=>(const SHA512& other) const {
        return normalized_ <=> other.normalized_;
    }
    bool operator==(const SHA512& other) const {
        return normalized_ == other.normalized_;
    }

    // Check a digest computed elsewhere (std::byte form).
    bool matches(std::span<const std::byte, kDigestBytes> digest) const;
    // Check a digest computed elsewhere (unsigned char form -- e.g. straight
    // out of OpenSSL's SHA512() or similar C-style crypto APIs).
    bool matches(std::span<const unsigned char, kDigestBytes> digest) const;

private:
    std::string raw_;
    std::string normalized_;
    std::array<std::byte, kDigestBytes> bytes_{};

    void parse_and_validate(const std::string& hex);
};

} // end sigmf_io namespace

#endif // SIGMF_IO_SHA512_H
