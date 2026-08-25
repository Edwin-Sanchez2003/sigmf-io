#include "sigmf_io/v1_2_6/spec_validator.h"

#include <stdexcept>
#include <optional>
#include <cstdint>
#include <vector>
#include <string>

#include "sigmf_io/global.h"
#include "sigmf_io/capture.h"
#include "sigmf_io/annotation.h"
#include "sigmf_io/metadata.h"
#include "sigmf_io/datatype.h"
#include "sigmf_io/sha512.h"
#include "sigmf_io/version.h"
#include "sigmf_io/datetime.h"
#include "sigmf_io/uuid.h"

namespace sigmf_io {

namespace v1_2_6 {

std::expected<void, std::string> SpecValidator::check_datatype(const std::string& datatype) const
{
    try {
        Datatype{datatype};
        return {};
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

std::expected<void, std::string> SpecValidator::check_sample_rate(double sample_rate) const
{
    if(sample_rate <= 0.0 || sample_rate > 1'000'000'000'000.0)
        return std::unexpected(
            "sample_rate must be greater than zero and less than or equal to 1 trillion. sample_rate: " + std::to_string(sample_rate));
    return {};
}

std::expected<void, std::string> SpecValidator::check_num_channels(int64_t num_channels) const
{
    if(num_channels < 1 || num_channels > INT64_MAX)
        return std::unexpected("num_channels must be greater than or equal to one. num_channels: " + std::to_string(num_channels));
    return {};
}

std::expected<void, std::string> SpecValidator::check_offset(int64_t offset) const
{
    if(offset < 0 || offset > INT64_MAX)
        return std::unexpected("offset must be greater than or equal to zero. offset: " + std::to_string(offset));
    return {};
}

std::expected<void, std::string> SpecValidator::check_sha512(const std::string& sha512) const
{
    try {
        SHA512{sha512};
        return {};
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

std::expected<void, std::string> SpecValidator::check_trailing_bytes(int64_t trailing_bytes) const
{
    if(trailing_bytes < 0 || trailing_bytes > INT64_MAX)
        return std::unexpected("trailing_bytes must be greater than or equal to zero. trailing_bytes: " + std::to_string(trailing_bytes));
    return {};
}

std::expected<void, std::string> SpecValidator::check_version(const std::string& version) const
{
    try {
        Version{version};
        return {};
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

// Capture checks
std::expected<void, std::string> SpecValidator::check_sample_start(int64_t sample_start) const
{
    if(sample_start < 0 || sample_start > INT64_MAX)
        return std::unexpected("sample_start must be greater than or equal to zero. sample_start: " + std::to_string(sample_start));
    return {};
}

std::expected<void, std::string> SpecValidator::check_datetime(const std::string& datetime) const
{
    try {
        Datetime{datetime};
        return {};
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

std::expected<void, std::string> SpecValidator::check_frequency(double frequency) const
{
    if((frequency < -1'000'000'000'000.0) || (frequency > 1'000'000'000'000.0))
        return std::unexpected("frequency must be less than -1 trillion or greater than 1 trillion. frequency: " + std::to_string(frequency));
    return {};
}

std::expected<void, std::string> SpecValidator::check_global_index(int64_t global_index) const
{
    if(global_index < 0 || global_index > INT64_MAX)
        return std::unexpected("global_index must be greater than or equal to zero. global_index: " + std::to_string(global_index));
    return {};
}

std::expected<void, std::string> SpecValidator::check_header_bytes(int64_t header_bytes) const
{
    if(header_bytes < 0 || header_bytes > INT64_MAX)
        return std::unexpected("header_bytes must be greater than or equal to zero. header_bytes: " + std::to_string(header_bytes));
    return {};
}

// Annotation checks
std::expected<void, std::string> SpecValidator::check_sample_count(int64_t sample_count) const
{
    if(sample_count < 0 || sample_count > INT64_MAX)
        return std::unexpected("sample_count must be greater than or equal to zero. sample_count: " + std::to_string(sample_count));
    return {};
}

std::expected<void, std::string> SpecValidator::check_freq_lower_edge(double freq_lower_edge) const
{
    if((freq_lower_edge < -1'000'000'000'000.0) || (freq_lower_edge > 1'000'000'000'000.0))
        return std::unexpected(
            "freq_lower_edge must be less than -1 trillion or greater than 1 trillion. freq_lower_edge: " + std::to_string(freq_lower_edge));
    return {};
}

std::expected<void, std::string> SpecValidator::check_freq_upper_edge(double freq_upper_edge) const
{
    if((freq_upper_edge < -1'000'000'000'000.0) || (freq_upper_edge > 1'000'000'000'000.0))
        return std::unexpected(
            "freq_upper_edge must be less than -1 trillion or greater than 1 trillion. freq_upper_edge: " + std::to_string(freq_upper_edge));
    return {};
}

std::expected<void, std::string> SpecValidator::check_uuid(const std::string& uuid) const
{
    try {
        UUID{uuid};
        return {};
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

std::expected<void, std::vector<std::string>> SpecValidator::check_global(const Global& global) const
{
    std::vector<std::string> errors;
    this->accumulate(errors, this->check_datatype(global.datatype()));
    std::optional<double> sample_rate = global.sample_rate();
    if(sample_rate.has_value())
        this->accumulate(errors, this->check_sample_rate(sample_rate));
    this->accumulate(errors, this->check_num_channels(global.num_channels()));
    this->accumulate(errors, this->check_offset(global.offset()));
    this->accumulate(errors, this->check_sha512(global.sha512()));
    this->accumulate(errors, this->check_trailing_bytes(global.trailing_bytes()));
    this->accumulate(errors, this->check_version(global.version()));

    if(!errors.empty())
        return std::unexpected(errors);
    return {};
}

std::expected<void, std::vector<std::string>> SpecValidator::check_capture(const Capture& capture) const
{
    std::vector<std::string> errors;
    this->accumulate(errors, this->check_sample_start(capture.sample_start()));
    this->accumulate(errors, this->check_datetime(capture.datetime()));
    this->accumulate(errors, this->check_frequency(capture.frequency()));
    this->accumulate(errors, this->check_global_index(capture.global_index()));
    this->accumulate(errors, this->check_header_bytes(capture.header_bytes()));
    //this->accumulate(errors, this->check_geolocation(capture.geolocation()));

    if(!errors.empty())
        return std::unexpected(errors);
    return {};
}


std::expected<void, std::vector<std::string>> SpecValidator::check_annotation(const Annotation& annotation) const
{
    std::vector<std::string> errors;
    this->accumulate(errors, this->check_sample_start(annotation.sample_start()));
    this->accumulate(errors, this->check_sample_count(annotation.sample_count()));
    this->accumulate(errors, this->check_freq_lower_edge(annotation.freq_lower_edge()));
    this->accumulate(errors, this->check_freq_upper_edge(annotation.freq_upper_edge()));
    this->accumulate(errors, this->check_uuid(annotation.uuid()));

    if(!errors.empty())
        return std::unexpected(errors);
    return {};
}


// validate a json object, assuming it is structured like a SigMF metadata file.
std::expected<void, std::vector<std::string>> SpecValidator::check_metadata(const Metadata& meta) const
{

}

} // end v1_2_6 namespace

} // end sigmf_io namespace
