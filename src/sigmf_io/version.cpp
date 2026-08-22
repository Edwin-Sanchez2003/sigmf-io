#include "sigmf_io/version.h"

#include <regex>
#include <stdexcept>
#include <string>

namespace sigmf_io {

Version::Version(const std::string& version)
{
    // Validate that version matches the format X.Y.Z where X, Y, Z are
    // non-negative integers (no leading zeros other than "0" itself).
    static const std::regex versionPattern(R"(^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)$)");
    std::smatch match;

    if (!std::regex_match(version, match, versionPattern))
    {
        throw std::invalid_argument(
            "SigMFVersion: invalid version string '" + version +
            "', expected format X.Y.Z");
    }

    this->major_ = std::stoi(match[1]);
    this->minor_ = std::stoi(match[2]);
    this->patch_ = std::stoi(match[3]);
    this->version_ = version;
}

} // end sigmf_io namespace
