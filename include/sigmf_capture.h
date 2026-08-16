#ifndef SIGMF_CAPTURE_H
#define SIGMF_CAPTURE_H

#include "json_base.h"
#include "SigMFGeoLocation.h"
#include "SigMFDateTime.h"
#include <jsoncons/json.hpp>
#include <optional>
#include <cstdint>
#include <string>

/*
 * SigMF Capture
 *
 * TODO: SigMFGeolocation.
 *
 */

class SigMFCapture : public JSONBase
{
public:
    explicit SigMFCapture(const jsoncons::json& data = jsoncons::json());

    // returns a jsoncons::json initialized with default SigMF values.
    static jsoncons::json default_data();

    int64_t sample_start() const;
    void set_sample_start(int64_t sample_start);
    std::optional<SigMFDateTime> datetime() const;
    void set_datetime(SigMFDateTime datetime);
    std::optional<double> frequency() const;
    void set_frequency(double frequency);
    std::optional<int64_t> global_index() const;
    void set_global_index(int64_t global_index);
    std::optional<int64_t> header_bytes() const;
    void set_header_bytes(int64_t header_bytes);
    // SigMFGeolocation getter/setter
};

#endif // SIGMF_CAPTURE_H
