#ifndef SIGMFGLOBAL_H
#define SIGMFGLOBAL_H

#include <optional>
#include <cstdint>
#include <string>
#include <vector>
#include "SigMFDataType.h"
#include "SigMFGeoLocation.h"
#include "SigMFExtension.h"

// defaults as defined by spec: https://sigmf.org/
struct SigMFGlobal
{
    SigMFDataType datatype{"cf32_le"};
    std::optional<double> sample_rate;
    std::optional<std::string> author;
    std::optional<std::string> collection;
    std::optional<std::string> dataset;
    std::optional<std::string> data_doi;    // TODO: Make custom class to enforce format.
    std::optional<std::string> description;
    std::optional<std::string> hw;
    std::optional<std::string> license;     // TODO: Make custom class to enforce format.
    std::optional<bool> metadata_only;
    std::optional<std::string> meta_doi;    // TODO: Make custom class to enforce format.
    int64_t num_channels{1};
    int64_t offset{0};
    std::optional<std::string> recorder;
    std::optional<std::string> sha512;      // TODO: Make custom class to enforce format.
    std::optional<int64_t> trailing_bytes;
    std::string version;                    // TODO: Make custom class to enforce format.
    std::optional<SigMFGeoLocation> geolocation;
    std::optional<std::vector<SigMFExtension>> extensions;
};

#endif // SIGMFGLOBAL_H
