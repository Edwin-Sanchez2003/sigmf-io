#ifndef SIGMF_IO_SPEC_VALIDATOR_BASE_H
#define SIGMF_IO_SPEC_VALIDATOR_BASE_H

#include <string>
#include <optional>
#include <expected>

#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonschema/jsonschema.hpp>

#include "sigmf_io/global.h"
#include "sigmf_io/capture.h"
#include "sigmf_io/annotation.h"
#include "sigmf_io/metadata.h"
#include "sigmf_io/sigmf_schema_data.h"

/*
 * SpecValidatorBase
 *
 * This class defines the interface that is used to validate fields within SigMF Metadata.
 * This class itself cannot be instantiated; It is meant to be a pure virtual interface that
 * child classes inherit from and implement based on a specific version of the SigMF Specification.
 */

namespace sigmf_io {

class SpecValidatorBase
{
public:
    explicit SpecValidatorBase(const std::string& version)
        :version_(version),
        validator_(SpecValidatorBase::make_validator())
    {}
    virtual ~SpecValidatorBase() = default;

    std::string version() const { return this->version_; }

    // Global checks
    virtual std::expected<void, std::string> check_datatype(const std::string& datatype) const = 0;
    virtual std::expected<void, std::string> check_sample_rate(double sample_rate) const = 0;
    /*
    virtual std::expected<void, std::string> check_author(const std::string& author) const = 0;
    virtual std::expected<void, std::string> check_collection(const std::string& collection) const = 0;
    virtual std::expected<void, std::string> check_dataset(const std::string& dataset) const = 0;
    virtual std::expected<void, std::string> check_data_doi(const std::string& data_doi) const = 0;
    virtual std::expected<void, std::string> check_description(const std::string& description) const = 0;
    virtual std::expected<void, std::string> check_hw(const std::string& hw) const = 0;
    virtual std::expected<void, std::string> check_license(const std::string& license) const = 0;
    virtual std::expected<void, std::string> check_metadata_only(bool metadata_only) const = 0;
    virtual std::expected<void, std::string> check_meta_doi(const std::string& meta_doi) const = 0;
    */
    virtual std::expected<void, std::string> check_num_channels(int64_t num_channels) const = 0;
    virtual std::expected<void, std::string> check_offset(int64_t offset) const = 0;
    /*
    virtual std::expected<void, std::string> check_recorder(const std::string& recorder) const = 0;
    */
    virtual std::expected<void, std::string> check_sha512(const std::string& sha512) const = 0;
    virtual std::expected<void, std::string> check_trailing_bytes(int64_t trailing_bytes) const = 0;
    virtual std::expected<void, std::string> check_version(const std::string& version) const = 0;
    // TODO: check Geolocation

    // Capture checks
    virtual std::expected<void, std::string> check_sample_start(int64_t sample_start) const = 0;
    virtual std::expected<void, std::string> check_datetime(const std::string& datetime) const = 0;
    virtual std::expected<void, std::string> check_frequency(double frequency) const = 0;
    virtual std::expected<void, std::string> check_global_index(int64_t global_index) const = 0;
    virtual std::expected<void, std::string> check_header_bytes(int64_t header_bytes) const = 0;
    // TODO: check Geolocation

    // Annotation checks
    virtual std::expected<void, std::string> check_sample_count(int64_t sample_count) const = 0;
    virtual std::expected<void, std::string> check_freq_lower_edge(double freq_lower_edge) const = 0;
    virtual std::expected<void, std::string> check_freq_upper_edge(double freq_upper_edge) const = 0;
    /*
    virtual std::expected<void, std::string> check_label(const std::string& label) const = 0;
    virtual std::expected<void, std::string> check_comment(const std::string& comment) const = 0;
    virtual std::expected<void, std::string> check_generator(const std::string& generator) const = 0;
    */
    virtual std::expected<void, std::string> check_uuid(const std::string& uuid) const = 0;

    // TODO: Add methods to check certain sets of fields, for convenience:
    virtual std::expected<void, std::vector<std::string>> check_global(const Global& global) const = 0;
    virtual std::expected<void, std::vector<std::string>> check_capture(const Capture& capture) const = 0;
    virtual std::expected<void, std::vector<std::string>> check_annotation(const Annotation& annotation) const = 0;

    // validate a json object, assuming it is structured like a SigMF metadata file.
    virtual std::expected<void, std::vector<std::string>> check_metadata(const Metadata& meta) const = 0;

    // convenience method to take a set of std::expected<> values & raise it as a single error during runtime.
    static void raise_errors(const std::expected<void, std::vector<std::string>>& validation_result);

    // TODO: inter-field spec validation.
private:
    const std::string version_;

    static jsoncons::jsonschema::json_schema<jsoncons::json> make_validator();

protected:
    jsoncons::jsonschema::json_schema<jsoncons::json> validator_;
    // Accumulates a single check's result into the running error list.
    // Returns nothing — errors vector is modified in place.
    static void accumulate(std::vector<std::string>& errors, const std::expected<void, std::string>& result);
};

jsoncons::jsonschema::json_schema<jsoncons::json> SpecValidatorBase::make_validator()
{
    jsoncons::json schema_json = jsoncons::json::parse(sigmf_io::SIGMF_SCHEMA_JSON);
    return jsoncons::jsonschema::make_json_schema(std::move(schema_json));
}

void SpecValidatorBase::accumulate(std::vector<std::string>& errors, const std::expected<void, std::string>& result)
{
    if (!result)
        errors.push_back(result.error());
}

void SpecValidatorBase::raise_errors(const std::expected<void, std::vector<std::string>>& validation_result)
{
    if (!validation_result.has_value()) // "this expected does NOT hold a success — it holds an error"
    {
        const std::vector<std::string>& messages = validation_result.error();

        for (const auto& msg : messages)
        {
            std::cerr << msg << '\n';
        }

        // Combine into one string for the exception, if you want a single message
        std::ostringstream oss;
        for (const auto& msg : messages)
        {
            oss << msg << '\n';
        }

        throw std::runtime_error(oss.str());
    }
}

} // end sigmf_io namespace

#endif // SIGMF_IO_SPEC_VALIDATOR_BASE_H
