#include "sigmf_capture.h"
#include "json_base.h"
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>


SigMFCapture::SigMFCapture(const jsoncons::json& data)
: JSONBase(SigMFCapture::default_data(), data)
{}


jsoncons::json SigMFCapture::default_data()
{
    jsoncons::json defaults;
    defaults["core:sample_start"] = 0;
    return defaults;
}


int64_t SigMFCapture::sample_start() const
{
    return this->get<int64_t>("/core:sample_start");
}


void SigMFCapture::set_sample_start(int64_t sample_start)
{
    this->set("/core:sample_start", sample_start);
}


std::optional<SigMFDateTime> SigMFCapture::datetime() const
{
    std::optional<std::string> datetime = this->get_optional<std::string>("/core:datetime");
    if(datetime.has_value()) {
        return SigMFDateTime(datetime.value());
    }
    return std::nullopt;
}


void SigMFCapture::set_datetime(SigMFDateTime datetime)
{
    this->set("/core:datetime", datetime.to_string());
}


std::optional<double> SigMFCapture::frequency() const
{
    return this->get_optional<double>("/core:frequency");
}


void SigMFCapture::set_frequency(double frequency)
{
    this->set("/core:frequency", frequency);
}


std::optional<int64_t> SigMFCapture::global_index() const
{
    return this->get_optional<int64_t>("/core:global_index");
}


void SigMFCapture::set_global_index(int64_t global_index)
{
    this->set("/core:global_index", global_index);
}


std::optional<int64_t> SigMFCapture::header_bytes() const
{
    return this->get_optional<int64_t>("/core:header_bytes");
}


void SigMFCapture::set_header_bytes(int64_t header_bytes)
{
    this->set("/core:header_bytes", header_bytes);
}
