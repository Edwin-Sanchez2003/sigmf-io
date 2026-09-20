#include "sigmf_io/capture.h"
#include "sigmf_io/json_base.h"

#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

#include "sigmf_io/spec_validator_base.h"
#include "sigmf_io/validation_context.h"
#include "sigmf_io/constants.h"

namespace sigmf_io {

Capture::Capture(const jsoncons::json& data, ValidationContext validation_context)
: JSONBase(Capture::default_data(), data, validation_context)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_errors(this->validation_context_.validator->check_capture(*this));
}


jsoncons::json Capture::default_data()
{
    jsoncons::json defaults;
    defaults[constants::SAMPLE_START] = 0;
    return defaults;
}


int64_t Capture::sample_start() const
{
    return this->get<int64_t>(sigmf_io::pointer(constants::SAMPLE_START));
}


void Capture::set_sample_start(int64_t sample_start)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_sample_start(sample_start));

    this->set(sigmf_io::pointer(constants::SAMPLE_START), sample_start);
}


std::optional<std::string> Capture::datetime() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::DATETIME));
}


void Capture::set_datetime(const std::string& datetime)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_datetime(datetime));

    this->set(sigmf_io::pointer(constants::DATETIME), datetime);
}


std::optional<double> Capture::frequency() const
{
    return this->get_optional<double>(sigmf_io::pointer(constants::FREQUENCY));
}


void Capture::set_frequency(double frequency)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_frequency(frequency));

    this->set(sigmf_io::pointer(constants::FREQUENCY), frequency);
}


std::optional<int64_t> Capture::global_index() const
{
    return this->get_optional<int64_t>(sigmf_io::pointer(constants::GLOBAL_INDEX));
}


void Capture::set_global_index(int64_t global_index)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_global_index(global_index));

    this->set(sigmf_io::pointer(constants::GLOBAL_INDEX), global_index);
}


int64_t Capture::header_bytes() const
{
    return this->get_optional<int64_t>(sigmf_io::pointer(constants::HEADER_BYTES)).value_or(0);
}


void Capture::set_header_bytes(int64_t header_bytes)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_header_bytes(header_bytes));

    this->set(sigmf_io::pointer(constants::HEADER_BYTES), header_bytes);
}

} // end sigmf_io namespaec
