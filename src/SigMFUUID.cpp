#include "SigMFUUID.h"
#include <regex>
#include <stdexcept>
#include <algorithm>
#include <cctype>

SigMFUUID::SigMFUUID(const std::string& uuid)
{
    if (!is_valid(uuid))
    {
        throw std::invalid_argument(
            "SigMFUUID: invalid UUID string '" + uuid + "', expected RFC-4122 format");
    }

    uuid_ = toLower(uuid);
    bytes_ = parseBytes(uuid_);
}

bool SigMFUUID::is_valid(const std::string& candidate)
{
    static const std::regex pattern(
        R"(^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$)");
    return std::regex_match(candidate, pattern);
}

bool SigMFUUID::is_nil() const
{
    return std::all_of(bytes_.begin(), bytes_.end(), [](uint8_t b) { return b == 0; });
}

std::array<uint8_t, 16> SigMFUUID::parseBytes(const std::string& canonical)
{
    std::string hex;
    hex.reserve(32);
    for (char c : canonical)
    {
        if (c != '-') hex.push_back(c);
    }

    std::array<uint8_t, 16> result{};
    for (size_t i = 0; i < 16; ++i)
    {
        result[i] = hexPairToByte(hex[2 * i], hex[2 * i + 1]);
    }
    return result;
}

std::string SigMFUUID::toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

uint8_t SigMFUUID::hexPairToByte(char hi, char lo)
{
    return static_cast<uint8_t>((nibble(hi) << 4) | nibble(lo));
}

uint8_t SigMFUUID::nibble(char c)
{
    if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
    return static_cast<uint8_t>(std::tolower(c) - 'a' + 10);
}
