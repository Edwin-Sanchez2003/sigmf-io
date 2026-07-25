#ifndef SIGMFCAPTURE_H
#define SIGMFCAPTURE_H

#include <optional>
#include <cstdint>
#include <string>
#include "SigMFGeoLocation.h"
#include "SigMFDateTime.h"

struct SigMFCapture
{
    int64_t sample_start{0};
    std::optional<SigMFDateTime> datetime;
    std::optional<double> frequency;
    std::optional<int64_t> global_index;        // NOTE: if omitted, SHOULD be treated as equal to sample_start.
    int64_t header_bytes{0};
    std::optional<SigMFGeoLocation> geolocation;
};

#endif // SIGMFCAPTURE_H
