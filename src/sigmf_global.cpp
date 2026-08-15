#include "sigmf_global.h"
#include "json_base.h"
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>
#include <jsoncons_ext/mergepatch/mergepatch.hpp>


SigMFGlobal::SigMFGlobal(const jsoncons::json& data)
    : JSONBase(SigMFGlobal::default_data(), data)
{}


jsoncons::json SigMFGlobal::default_data()
{
    jsoncons::json defaults;
    defaults["global"]["core:datatype"] = "cf32_le";
    defaults["global"]["core:num_channels"] = 1;
    defaults["global"]["core:offset"] = 0;
    defaults["global"]["core:version"] = "1.2.6";
    defaults["global"]["core:extensions"] = jsoncons::json(jsoncons::json_array_arg);
    return defaults;
}


SigMFDataType SigMFGlobal::datatype() const {}


void SigMFGlobal::set_datatype(const SigMFDataType& datatype) {}


std::optional<double> SigMFGlobal::sample_rate() const {}


void SigMFGlobal::set_sample_rate(const double sample_rate) {}


std::optional<std::string> SigMFGlobal::author() const {}


void SigMFGlobal::set_author(const std::string& author) {}


std::optional<std::string> SigMFGlobal::collection() const {}


void SigMFGlobal::set_collection(const std::string& collection) {}


std::optional<std::string> SigMFGlobal::dataset() const {}


void SigMFGlobal::set_dataset(const std::string& dataset) {}


std::optional<std::string> SigMFGlobal::data_doi() const {}


void SigMFGlobal::set_data_doi(const std::string& data_doi) {}


std::optional<std::string> SigMFGlobal::description() const {}


void SigMFGlobal::set_description(const std::string& description) {}


std::optional<std::string> SigMFGlobal::hw() const {}


void SigMFGlobal::set_hw(const std::string& hw) {}


std::optional<std::string> SigMFGlobal::license() const {}


void SigMFGlobal::set_license(const std::string& license) {}


std::optional<bool> SigMFGlobal::matadata_only() const {}


void SigMFGlobal::set_metadata_only(const bool metadata_only) {}


std::optional<std::string> SigMFGlobal::meta_doi() const {}


void SigMFGlobal::set_meta_doi(const std::string& meta_doi) {}


int64_t SigMFGlobal::num_channels() const {}


void SigMFGlobal::set_num_channels(const int64_t num_channels) {}


int64_t SigMFGlobal::offset() const {}


void SigMFGlobal::set_offset(int64_t offset) {}


std::optional<std::string> SigMFGlobal::recorder() const {}


void SigMFGlobal::set_recorder(const std::string& recorder) {}


std::optional<SigMFSHA512> SigMFGlobal::sha512() const {}


void SigMFGlobal::set_sha512(const SigMFSHA512& sha512) {}


std::optional<int64_t> SigMFGlobal::trailing_bytes() const {}


void SigMFGlobal::set_trailing_bytes(const int64_t trailing_bytes) {}


std::string SigMFGlobal::version() const {}


void SigMFGlobal::set_version(const std::string& version) {}


SigMFGeoLocation SigMFGlobal::geolocation() const {}


void SigMFGlobal::set_geolocation(const SigMFGeoLocation& geolocation) {}


std::vector<SigMFExtension> SigMFGlobal::extensions() const {}


void SigMFGlobal::set_extensions(const std::vector<SigMFExtension>& extensions) {}
