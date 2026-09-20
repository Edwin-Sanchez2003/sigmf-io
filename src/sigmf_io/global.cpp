#include "sigmf_io/global.h"

#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

#include "sigmf_io/datatype.h"
#include "sigmf_io/sha512.h"
#include "sigmf_io/datetime.h"
#include "sigmf_io/uuid.h"
#include "sigmf_io/json_base.h"
#include "sigmf_io/spec_validator_base.h"
#include "sigmf_io/validation_context.h"
#include "sigmf_io/constants.h"

namespace sigmf_io {

Global::Global(const jsoncons::json& data, ValidationContext validation_context)
    : JSONBase(Global::default_data(), data, validation_context)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_errors(this->validation_context_.validator->check_global(*this));
}


jsoncons::json Global::default_data()
{
    jsoncons::json defaults;
    defaults[constants::DATATYPE] = "cf32_le";
    defaults[constants::NUM_CHANNELS] = 1;
    defaults[constants::OFFSET] = 0;
    defaults[constants::VERSION] = "1.2.6";
    defaults[constants::EXTENSIONS] = jsoncons::json(jsoncons::json_array_arg);
    return defaults;
}


std::string Global::datatype() const
{
    return this->get<std::string>(sigmf_io::pointer(constants::DATATYPE));
}


void Global::set_datatype(const std::string& datatype)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_datatype(datatype));

    this->set(sigmf_io::pointer(constants::DATATYPE), datatype);
}


std::optional<double> Global::sample_rate() const
{
    return this->get_optional<double>(sigmf_io::pointer(constants::SAMPLE_RATE));
}


void Global::set_sample_rate(const double sample_rate)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_sample_rate(sample_rate));

    this->set(sigmf_io::pointer(constants::SAMPLE_RATE), sample_rate);
}


std::optional<std::string> Global::author() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::AUTHOR));
}


void Global::set_author(const std::string& author)
{
    this->set(sigmf_io::pointer(constants::AUTHOR), author);
}


std::optional<std::string> Global::collection() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::COLLECTION));
}


void Global::set_collection(const std::string& collection)
{
    this->set(sigmf_io::pointer(constants::COLLECTION), collection);
}


std::optional<std::string> Global::dataset() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::DATASET));
}


void Global::set_dataset(const std::string& dataset)
{
    this->set(sigmf_io::pointer(constants::DATASET), dataset);
}


std::optional<std::string> Global::data_doi() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::DATA_DOI));
}


void Global::set_data_doi(const std::string& data_doi)
{
    this->set(sigmf_io::pointer(constants::DATA_DOI), data_doi);
}


std::optional<std::string> Global::description() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::DESCRIPTION));
}


void Global::set_description(const std::string& description)
{
    this->set(sigmf_io::pointer(constants::DESCRIPTION), description);
}


std::optional<std::string> Global::hw() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::HW));
}


void Global::set_hw(const std::string& hw)
{
    this->set(sigmf_io::pointer(constants::HW), hw);
}


std::optional<std::string> Global::license() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::LICENSE));
}


void Global::set_license(const std::string& license)
{
    this->set(sigmf_io::pointer(constants::LICENSE), license);
}


std::optional<bool> Global::matadata_only() const
{
    return this->get_optional<bool>(sigmf_io::pointer(constants::METADATA_ONLY));
}


void Global::set_metadata_only(const bool metadata_only)
{
    this->set(sigmf_io::pointer(constants::METADATA_ONLY), metadata_only);
}


std::optional<std::string> Global::meta_doi() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::META_DOI));
}


void Global::set_meta_doi(const std::string& meta_doi)
{
    this->set(sigmf_io::pointer(constants::META_DOI), meta_doi);
}


int64_t Global::num_channels() const
{
    return this->get_optional<int64_t>(sigmf_io::pointer(constants::NUM_CHANNELS)).value_or(1);
}


void Global::set_num_channels(const int64_t num_channels)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_num_channels(num_channels));

    this->set(sigmf_io::pointer(constants::NUM_CHANNELS), num_channels);
}


int64_t Global::offset() const
{
    return this->get_optional<int64_t>(sigmf_io::pointer(constants::OFFSET)).value_or(0);
}


void Global::set_offset(int64_t offset)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_offset(offset));

    this->set(sigmf_io::pointer(constants::OFFSET), offset);
}


std::optional<std::string> Global::recorder() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::RECORDER));
}


void Global::set_recorder(const std::string& recorder)
{
    return this->set(sigmf_io::pointer(constants::RECORDER), recorder);
}


std::optional<std::string> Global::sha512() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::SHA512));
}


void Global::set_sha512(const std::string& sha512)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_sha512(sha512));

    this->set(sigmf_io::pointer(constants::SHA512), sha512);
}


int64_t Global::trailing_bytes() const
{
    return this->get_optional<int64_t>(sigmf_io::pointer(constants::TRAILING_BYTES)).value_or(0);
}


void Global::set_trailing_bytes(const int64_t trailing_bytes)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_trailing_bytes(trailing_bytes));

    this->set(sigmf_io::pointer(constants::TRAILING_BYTES), trailing_bytes);
}


std::string Global::version() const
{
    return this->get<std::string>(sigmf_io::pointer(constants::VERSION));
}


void Global::set_version(const std::string& version)
{
    this->set(sigmf_io::pointer(constants::VERSION), version);
}


// std::optional<SigMFGeoLocation> Global::geolocation() const
// {
//     std::optional<jsoncons::json> geolocation = this->get_optional<jsoncons::json>("/core:geolocation");
//     if(geolocation.has_value()) {
//         return SigMFGeoLocation(geolocation);
//     }
//     return std::nullopt;
// }


// void Global::set_geolocation(const SigMFGeoLocation& geolocation) {}


// std::vector<SigMFExtension> Global::extensions() const {}


// void Global::set_extensions(const std::vector<SigMFExtension>& extensions) {}

} // end sigmf_io namespace
