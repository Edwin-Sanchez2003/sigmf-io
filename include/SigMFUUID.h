#ifndef SIGMFUUID_H
#define SIGMFUUID_H

#include <string>
#include <cstdint>
#include <array>

class SigMFUUID
{
public:
    // Construct from an existing RFC-4122 string; throws if invalid.
    explicit SigMFUUID(const std::string& uuid);

    // Returns the canonical 8-4-4-4-12 lowercase hyphenated string.
    std::string uuid() const { return uuid_; }

    // True if this is the RFC-4122 "nil" UUID (all zeros).
    bool is_nil() const;

    // Static validity check without constructing an object.
    static bool is_valid(const std::string& candidate);

    // Raw 16-byte representation, in the order they appear in the
    // canonical string (i.e. not necessarily network byte order for
    // any particular field, just the straightforward hex-pair decode).
    std::array<uint8_t, 16> bytes() const { return bytes_; }

    bool operator==(const SigMFUUID& other) const { return bytes_ == other.bytes_; }
    bool operator!=(const SigMFUUID& other) const { return !(*this == other); }
    bool operator<(const SigMFUUID& other) const { return bytes_ < other.bytes_; }

private:
    std::string uuid_;
    std::array<uint8_t, 16> bytes_;

    static std::array<uint8_t, 16> parseBytes(const std::string& canonical);
    static std::string toLower(std::string s);
    static uint8_t hexPairToByte(char hi, char lo);
    static uint8_t nibble(char c);
};

#endif // SIGMFUUID_H
