#ifndef SIGMFGLOBAL_H
#define SIGMFGLOBAL_H

#include <optional>
#include <cstdint>
#include <string>
#include <vector>
#include "SigMFDataType.h"
#include "SigMFGeoLocation.h"
#include "SigMFExtension.h"
#include "SigMFSHA512.h"
#include "SigMFVersion.h"

// defaults as defined by spec: https://sigmf.org/
struct SigMFGlobal
{
    SigMFDataType datatype{"cf32_le"};
    std::optional<double> sample_rate;
    std::optional<std::string> author;
    std::optional<std::string> collection;
    std::optional<std::string> dataset;
    std::optional<std::string> data_doi;
    std::optional<std::string> description;
    std::optional<std::string> hw;
    std::optional<std::string> license;
    std::optional<bool> metadata_only;
    std::optional<std::string> meta_doi;
    int64_t num_channels{1};
    int64_t offset{0};
    std::optional<std::string> recorder;
    std::optional<SigMFSHA512> sha512;
    std::optional<int64_t> trailing_bytes;
    SigMFVersion version;
    std::optional<SigMFGeoLocation> geolocation;            // TODO: extend to allow for global's older version of captures...
    std::optional<std::vector<SigMFExtension>> extensions;  // TODO: implement SigMFExtensions interface...

    jsoncons::json extra = jsoncons::json::object();
};

#endif // SIGMFGLOBAL_H
