#include "sigmf_io/uuid.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>

namespace sigmf_io {

UUID::UUID(const std::string& uuid)
{
    if (!is_valid(uuid)) {
        throw std::invalid_argument(
            "UUID: invalid UUID string '" + uuid + "', expected RFC-4122 format");
    }
    uuid_ = toLower(uuid);
    bytes_ = parseBytes(uuid_);
}

bool UUID::is_valid(const std::string& candidate)
{
    static const std::regex pattern(
        R"(^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$)");
    return std::regex_match(candidate, pattern);
}

bool UUID::is_nil() const
{
    return std::all_of(bytes_.begin(), bytes_.end(),
                       [](std::byte b) { return b == std::byte{0}; });
}

std::array<std::byte, 16> UUID::parseBytes(const std::string& canonical)
{
    std::string hex;
    hex.reserve(32);
    for (char c : canonical) {
        if (c != '-') hex.push_back(c);
    }
    std::array<std::byte, 16> result{};
    for (size_t i = 0; i < 16; ++i) {
        result[i] = static_cast<std::byte>(hexPairToByte(hex[2 * i], hex[2 * i + 1]));
    }
    return result;
}

std::string UUID::toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

uint8_t UUID::hexPairToByte(char hi, char lo)
{
    return static_cast<uint8_t>((nibble(hi) << 4) | nibble(lo));
}

uint8_t UUID::nibble(char c)
{
    if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
    return static_cast<uint8_t>(std::tolower(c) - 'a' + 10);
}

} // end sigmf_io namespace
