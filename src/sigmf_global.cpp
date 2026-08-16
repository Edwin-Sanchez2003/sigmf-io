#include "sigmf_global.h"
#include "json_base.h"
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>


SigMFGlobal::SigMFGlobal(const jsoncons::json& data)
    : JSONBase(SigMFGlobal::default_data(), data)
{}


jsoncons::json SigMFGlobal::default_data()
{
    jsoncons::json defaults;
    defaults["core:datatype"] = "cf32_le";
    defaults["core:num_channels"] = 1;
    defaults["core:offset"] = 0;
    defaults["core:version"] = "1.2.6";
    defaults["core:extensions"] = jsoncons::json(jsoncons::json_array_arg);
    return defaults;
}


SigMFDataType SigMFGlobal::datatype() const
{
    return SigMFDataType(this->get<std::string>("/core:datatype"));
}


void SigMFGlobal::set_datatype(const SigMFDataType& datatype)
{
    this->set("/core:datatype", datatype.to_string());
}


std::optional<double> SigMFGlobal::sample_rate() const
{
    return this->get_optional<double>("/core:sample_rate");
}


void SigMFGlobal::set_sample_rate(const double sample_rate)
{
    this->set("/core:sample_rate", sample_rate);
}


std::optional<std::string> SigMFGlobal::author() const
{
    return this->get_optional<std::string>("/core:author");
}


void SigMFGlobal::set_author(const std::string& author)
{
    this->set("/core:author", author);
}


std::optional<std::string> SigMFGlobal::collection() const
{
    return this->get_optional<std::string>("/core:collection");
}


void SigMFGlobal::set_collection(const std::string& collection)
{
    this->set("/core:collection", collection);
}


std::optional<std::string> SigMFGlobal::dataset() const
{
    return this->get_optional<std::string>("/core:collection");
}


void SigMFGlobal::set_dataset(const std::string& dataset)
{
    this->set("/core:dataset", dataset);
}


std::optional<std::string> SigMFGlobal::data_doi() const
{
    return this->get_optional<std::string>("/core:data_doi");
}


void SigMFGlobal::set_data_doi(const std::string& data_doi)
{
    this->set("/core:data_doi", data_doi);
}


std::optional<std::string> SigMFGlobal::description() const
{
    return this->get_optional<std::string>("/core:description");
}


void SigMFGlobal::set_description(const std::string& description)
{
    this->set("/core:description", description);
}


std::optional<std::string> SigMFGlobal::hw() const
{
    return this->get_optional<std::string>("/core:hw");
}


void SigMFGlobal::set_hw(const std::string& hw)
{
    this->set("/core:hw", hw);
}


std::optional<std::string> SigMFGlobal::license() const
{
    return this->get_optional<std::string>("/core:license");
}


void SigMFGlobal::set_license(const std::string& license)
{
    this->set("/core:license", license);
}


std::optional<bool> SigMFGlobal::matadata_only() const
{
    return this->get_optional<bool>("/core:metadata_only");
}


void SigMFGlobal::set_metadata_only(const bool metadata_only)
{
    this->set("/core:metadata_only", metadata_only);
}


std::optional<std::string> SigMFGlobal::meta_doi() const
{
    return this->get_optional<std::string>("/core:meta_doi");
}


void SigMFGlobal::set_meta_doi(const std::string& meta_doi)
{
    this->set("/core:meta_doi", meta_doi);
}


int64_t SigMFGlobal::num_channels() const
{
    return this->get_optional<int64_t>("/core:num_channels").value_or(1);
}


void SigMFGlobal::set_num_channels(const int64_t num_channels)
{
    this->set("/core:num_channels", num_channels);
}


int64_t SigMFGlobal::offset() const
{
    return this->get_optional<int64_t>("/core:offset").value_or(0);
}


void SigMFGlobal::set_offset(int64_t offset)
{
    this->set("/core:offset", offset);
}


std::optional<std::string> SigMFGlobal::recorder() const
{
    return this->get_optional<std::string>("/core:recorder");
}


void SigMFGlobal::set_recorder(const std::string& recorder)
{
    return this->set("/core:recorder", recorder);
}


std::optional<SigMFSHA512> SigMFGlobal::sha512() const
{
    std::optional<std::string> sha512 = this->get_optional<std::string>("/core:sha512");
    if(sha512.has_value()) {
        return SigMFSHA512(sha512.value());
    }
    return std::nullopt;
}


void SigMFGlobal::set_sha512(const SigMFSHA512& sha512)
{
    this->set("/core:sha512", sha512.to_string());
}


std::optional<int64_t> SigMFGlobal::trailing_bytes() const
{
    return this->get_optional<int64_t>("/core:trailing_bytes");
}


void SigMFGlobal::set_trailing_bytes(const int64_t trailing_bytes)
{
    this->set("/core:trailing_bytes", trailing_bytes);
}


std::string SigMFGlobal::version() const
{
    return this->get<std::string>("/core:version");
}


void SigMFGlobal::set_version(const std::string& version)
{
    this->set("/core:version", version);
}


// std::optional<SigMFGeoLocation> SigMFGlobal::geolocation() const
// {
//     std::optional<jsoncons::json> geolocation = this->get_optional<jsoncons::json>("/core:geolocation");
//     if(geolocation.has_value()) {
//         return SigMFGeoLocation(geolocation);
//     }
//     return std::nullopt;
// }


// void SigMFGlobal::set_geolocation(const SigMFGeoLocation& geolocation) {}


// std::vector<SigMFExtension> SigMFGlobal::extensions() const {}


// void SigMFGlobal::set_extensions(const std::vector<SigMFExtension>& extensions) {}
