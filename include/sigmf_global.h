#ifndef SIGMF_GLOBAL_H
#define SIGMF_GLOBAL_H

#include "json_base.h"
#include "SigMFDataType.h"
#include "SigMFSHA512.h"
#include "SigMFGeoLocation.h"
#include "SigMFExtension.h"
#include <jsoncons/json.hpp>
#include <optional>
#include <cstdint>
#include <string>

class SigMFGlobal : public JSONBase
{
public:
    explicit SigMFGlobal(const jsoncons::json& data = jsoncons::json());

    // returns a jsoncons::json initialized with default SigMF values.
    static jsoncons::json default_data();

    // core namespace getters & setters.
    SigMFDataType datatype() const;
    void set_datatype(const SigMFDataType& datatype);
    std::optional<double> sample_rate() const;
    void set_sample_rate(const double sample_rate);
    std::optional<std::string> author() const;
    void set_author(const std::string& author);
    std::optional<std::string> collection() const;
    void set_collection(const std::string& collection);
    std::optional<std::string> dataset() const;
    void set_dataset(const std::string& dataset);
    std::optional<std::string> data_doi() const;
    void set_data_doi(const std::string& data_doi);
    std::optional<std::string> description() const;
    void set_description(const std::string& description);
    std::optional<std::string> hw() const;
    void set_hw(const std::string& hw);
    std::optional<std::string> license() const;
    void set_license(const std::string& license);
    std::optional<bool> matadata_only() const;
    void set_metadata_only(const bool metadata_only);
    std::optional<std::string> meta_doi() const;
    void set_meta_doi(const std::string& meta_doi);
    int64_t num_channels() const;
    void set_num_channels(const int64_t num_channels);
    int64_t offset() const;
    void set_offset(int64_t offset);
    std::optional<std::string> recorder() const;
    void set_recorder(const std::string& recorder);
    std::optional<SigMFSHA512> sha512() const;
    void set_sha512(const SigMFSHA512& sha512);
    std::optional<int64_t> trailing_bytes() const;
    void set_trailing_bytes(const int64_t trailing_bytes);
    std::string version() const;
    void set_version(const std::string& version);
    // std::optional<SigMFGeoLocation> geolocation() const;
    // void set_geolocation(const SigMFGeoLocation& geolocation);
    // std::vector<SigMFExtension> extensions() const;
    // void set_extensions(const std::vector<SigMFExtension>& extensions);
};

#endif // SIGMF_GLOBAL_H
