#include "sigmf_io/capture.h"
#include "sigmf_io/json_base.h"
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

namespace sigmf_io {

Capture::Capture(const jsoncons::json& data)
: JSONBase(Capture::default_data(), data)
{}


jsoncons::json Capture::default_data()
{
    jsoncons::json defaults;
    defaults["core:sample_start"] = 0;
    return defaults;
}


int64_t Capture::sample_start() const
{
    return this->get<int64_t>("/core:sample_start");
}


void Capture::set_sample_start(int64_t sample_start)
{
    this->set("/core:sample_start", sample_start);
}


std::optional<std::string> Capture::datetime() const
{
    return this->get_optional<std::string>("/core:datetime");
}


void Capture::set_datetime(const std::string& datetime)
{
    this->set("/core:datetime", datetime);
}


std::optional<double> Capture::frequency() const
{
    return this->get_optional<double>("/core:frequency");
}


void Capture::set_frequency(double frequency)
{
    this->set("/core:frequency", frequency);
}


std::optional<int64_t> Capture::global_index() const
{
    return this->get_optional<int64_t>("/core:global_index");
}


void Capture::set_global_index(int64_t global_index)
{
    this->set("/core:global_index", global_index);
}


std::optional<int64_t> Capture::header_bytes() const
{
    return this->get_optional<int64_t>("/core:header_bytes");
}


void Capture::set_header_bytes(int64_t header_bytes)
{
    this->set("/core:header_bytes", header_bytes);
}

} // end sigmf_io namespaec
