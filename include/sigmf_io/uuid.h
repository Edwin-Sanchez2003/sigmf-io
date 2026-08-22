#ifndef SIGMF_IO_UUID_H
#define SIGMF_IO_UUID_H
#include <array>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace sigmf_io {

class UUID
{
public:
    explicit UUID(const std::string& uuid);

    std::string uuid() const { return uuid_; }
    bool is_nil() const;
    static bool is_valid(const std::string& candidate);

    // Raw 16-byte representation, in the order they appear in the
    // canonical string (i.e. not necessarily network byte order for
    // any particular field, just the straightforward hex-pair decode).
    std::array<std::byte, 16> bytes() const { return bytes_; }

    // Zero-cost view for interop with C-style APIs (OpenSSL, Qt, POSIX,
    // etc.) that expect unsigned char buffers rather than std::byte.
    std::span<const unsigned char, 16> as_bytes_view() const {
        return std::span<const unsigned char, 16>(
            reinterpret_cast<const unsigned char*>(bytes_.data()), 16);
    }

    std::strong_ordering operator<=>(const UUID& other) const { return bytes_ <=> other.bytes_; }
    bool operator==(const UUID& other) const { return bytes_ == other.bytes_; }

private:
    std::string uuid_;
    std::array<std::byte, 16> bytes_;

    static std::array<std::byte, 16> parseBytes(const std::string& canonical);
    static std::string toLower(std::string s);
    static uint8_t hexPairToByte(char hi, char lo); // numeric result -- real magnitude
    static uint8_t nibble(char c);                  // numeric result -- real hex digit value
};

} // end sigmf_io namespace

#endif // SIGMF_IO_UUID_H
