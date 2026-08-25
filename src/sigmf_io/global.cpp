#include "sigmf_io/global.h"
#include "sigmf_io/datatype.h"
#include "sigmf_io/sha512.h"
#include "sigmf_io/datetime.h"
#include "sigmf_io/uuid.h"
#include "sigmf_io/json_base.h"
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

namespace sigmf_io {

Global::Global(const jsoncons::json& data)
    : JSONBase(Global::default_data(), data)
{}


jsoncons::json Global::default_data()
{
    jsoncons::json defaults;
    defaults["core:datatype"] = "cf32_le";
    defaults["core:num_channels"] = 1;
    defaults["core:offset"] = 0;
    defaults["core:version"] = "1.2.6";
    defaults["core:extensions"] = jsoncons::json(jsoncons::json_array_arg);
    return defaults;
}


std::string Global::datatype() const
{
    return this->get<std::string>("/core:datatype");
}


void Global::set_datatype(const Datatype& datatype)
{
    this->set("/core:datatype", datatype.to_string());
}


std::optional<double> Global::sample_rate() const
{
    return this->get_optional<double>("/core:sample_rate");
}


void Global::set_sample_rate(const double sample_rate)
{
    this->set("/core:sample_rate", sample_rate);
}


std::optional<std::string> Global::author() const
{
    return this->get_optional<std::string>("/core:author");
}


void Global::set_author(const std::string& author)
{
    this->set("/core:author", author);
}


std::optional<std::string> Global::collection() const
{
    return this->get_optional<std::string>("/core:collection");
}


void Global::set_collection(const std::string& collection)
{
    this->set("/core:collection", collection);
}


std::optional<std::string> Global::dataset() const
{
    return this->get_optional<std::string>("/core:collection");
}


void Global::set_dataset(const std::string& dataset)
{
    this->set("/core:dataset", dataset);
}


std::optional<std::string> Global::data_doi() const
{
    return this->get_optional<std::string>("/core:data_doi");
}


void Global::set_data_doi(const std::string& data_doi)
{
    this->set("/core:data_doi", data_doi);
}


std::optional<std::string> Global::description() const
{
    return this->get_optional<std::string>("/core:description");
}


void Global::set_description(const std::string& description)
{
    this->set("/core:description", description);
}


std::optional<std::string> Global::hw() const
{
    return this->get_optional<std::string>("/core:hw");
}


void Global::set_hw(const std::string& hw)
{
    this->set("/core:hw", hw);
}


std::optional<std::string> Global::license() const
{
    return this->get_optional<std::string>("/core:license");
}


void Global::set_license(const std::string& license)
{
    this->set("/core:license", license);
}


std::optional<bool> Global::matadata_only() const
{
    return this->get_optional<bool>("/core:metadata_only");
}


void Global::set_metadata_only(const bool metadata_only)
{
    this->set("/core:metadata_only", metadata_only);
}


std::optional<std::string> Global::meta_doi() const
{
    return this->get_optional<std::string>("/core:meta_doi");
}


void Global::set_meta_doi(const std::string& meta_doi)
{
    this->set("/core:meta_doi", meta_doi);
}


int64_t Global::num_channels() const
{
    return this->get_optional<int64_t>("/core:num_channels").value_or(1);
}


void Global::set_num_channels(const int64_t num_channels)
{
    this->set("/core:num_channels", num_channels);
}


int64_t Global::offset() const
{
    return this->get_optional<int64_t>("/core:offset").value_or(0);
}


void Global::set_offset(int64_t offset)
{
    this->set("/core:offset", offset);
}


std::optional<std::string> Global::recorder() const
{
    return this->get_optional<std::string>("/core:recorder");
}


void Global::set_recorder(const std::string& recorder)
{
    return this->set("/core:recorder", recorder);
}


std::optional<std::string> Global::sha512() const
{
    return this->get_optional<std::string>("/core:sha512");
}


void Global::set_sha512(const SHA512& sha512)
{
    this->set("/core:sha512", sha512.to_string());
}


std::optional<int64_t> Global::trailing_bytes() const
{
    return this->get_optional<int64_t>("/core:trailing_bytes");
}


void Global::set_trailing_bytes(const int64_t trailing_bytes)
{
    this->set("/core:trailing_bytes", trailing_bytes);
}


std::string Global::version() const
{
    return this->get<std::string>("/core:version");
}


void Global::set_version(const std::string& version)
{
    this->set("/core:version", version);
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
