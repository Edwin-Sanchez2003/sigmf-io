#ifndef SIGMFCAPTURE_H
#define SIGMFCAPTURE_H

#include "SigMFGeoLocation.h"
#include "SigMFDateTime.h"
#include <jsoncons/json.hpp>
#include <optional>
#include <cstdint>
#include <string>

struct SigMFCapture
{
    int64_t sample_start{0};
    std::optional<SigMFDateTime> datetime;
    std::optional<double> frequency;
    std::optional<int64_t> global_index;    // NOTE: if omitted, SHOULD be treated as equal to sample_start.
    int64_t header_bytes{0};                // NOTE: if omitted, SHOULD be treated as zero. TODO: If included, is NCD... decide how to handle this...
    std::optional<SigMFGeoLocation> geolocation;

    jsoncons::json extra = jsoncons::json::object();
};

#endif // SIGMFCAPTURE_H
