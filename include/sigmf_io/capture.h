#ifndef SIGMF_IO_CAPTURE_H
#define SIGMF_IO_CAPTURE_H


#include <cstdint>
#include <optional>
#include <string>

#include <jsoncons/json.hpp>

#include "sigmf_io/json_base.h"
#include "sigmf_io/geolocation.h"
#include "sigmf_io/datetime.h"

/*
 * SigMF Capture
 *
 * TODO: SigMFGeolocation.
 *
 */

namespace sigmf_io {

class Capture : public JSONBase
{
public:
    explicit Capture(const jsoncons::json& data = jsoncons::json());

    // returns a jsoncons::json initialized with default SigMF values.
    static jsoncons::json default_data();

    int64_t sample_start() const;
    void set_sample_start(int64_t sample_start);
    std::optional<std::string> datetime() const;
    void set_datetime(const std::string& datetime);
    std::optional<double> frequency() const;
    void set_frequency(double frequency);
    std::optional<int64_t> global_index() const;
    void set_global_index(int64_t global_index);
    std::optional<int64_t> header_bytes() const;
    void set_header_bytes(int64_t header_bytes);
    // SigMFGeolocation getter/setter
};

} // end sigmf_io namespace

// INTENTIONAL: Including capture_traits.h for jsoncons::json support.
// The errors for when a user tries to use jsoncons::json j.as<Capture>() without
// the capture_trait definition results in unclear errors, as jsoncons::json
// will default back to generic/reflection-based trait resolution, which won't
// point to this missing definition being the actual mistake. This forces users
// of a capture to include this definition by default in-scope, within the
// jsoncons::json namespace.
#include "sigmf_io/capture_traits.h"

#endif // SIGMF_IO_CAPTURE_H
