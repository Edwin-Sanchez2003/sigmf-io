#ifndef SIGMF_IO_SPEC_VALIDATOR_H
#define SIGMF_IO_SPEC_VALIDATOR_H

#include <expected>
#include <string>

#include "sigmf_io/spec_validator_base.h"
#include "sigmf_io/global.h"
#include "sigmf_io/capture.h"
#include "sigmf_io/annotation.h"
#include "sigmf_io/metadata.h"

namespace sigmf_io {

namespace v1_2_6 {

class SpecValidator : public SpecValidatorBase
{
public:
    SpecValidator() : SpecValidatorBase("1.2.6") {}

    // Specification Field Validation Methods - must implement all base class methods.
    // Global checks
    virtual std::expected<void, std::string> check_datatype(const std::string& datatype) const override;
    virtual std::expected<void, std::string> check_sample_rate(double sample_rate) const override;
    // virtual std::expected<void, std::string> check_author(const std::string& author) const override;
    // virtual std::expected<void, std::string> check_collection(const std::string& collection) const override;
    // virtual std::expected<void, std::string> check_dataset(const std::string& dataset) const override;
    // virtual std::expected<void, std::string> check_data_doi(const std::string& data_doi) const override;
    // virtual std::expected<void, std::string> check_description(const std::string& description) const override;
    // virtual std::expected<void, std::string> check_hw(const std::string& hw) const override;
    // virtual std::expected<void, std::string> check_license(const std::string& license) const override;
    // virtual std::expected<void, std::string> check_metadata_only(bool metadata_only) const override;
    // virtual std::expected<void, std::string> check_meta_doi(const std::string& meta_doi) const override;
    virtual std::expected<void, std::string> check_num_channels(int64_t num_channels) const override;
    virtual std::expected<void, std::string> check_offset(int64_t offset) const override;
    // virtual std::expected<void, std::string> check_recorder(const std::string& recorder) const override;
    virtual std::expected<void, std::string> check_sha512(const std::string& sha512) const override;
    virtual std::expected<void, std::string> check_trailing_bytes(int64_t trailing_bytes) const override;
    virtual std::expected<void, std::string> check_version(const std::string& version) const override;
    // Check Geolocation

    // Capture checks
    virtual std::expected<void, std::string> check_sample_start(int64_t sample_start) const override;
    virtual std::expected<void, std::string> check_datetime(const std::string& datetime) const override;
    virtual std::expected<void, std::string> check_frequency(double frequency) const override;
    virtual std::expected<void, std::string> check_global_index(int64_t global_index) const override;
    virtual std::expected<void, std::string> check_header_bytes(int64_t header_bytes) const override;
    // Check Geolocation

    // Annotation checks
    virtual std::expected<void, std::string> check_sample_count(int64_t sample_count) const override;
    virtual std::expected<void, std::string> check_freq_lower_edge(double freq_lower_edge) const override;
    virtual std::expected<void, std::string> check_freq_upper_edge(double freq_upper_edge) const override;
    // virtual std::expected<void, std::string> check_label(const std::string& label) const override;
    // virtual std::expected<void, std::string> check_comment(const std::string& comment) const override;
    // virtual std::expected<void, std::string> check_generator(const std::string& generator) const override;
    virtual std::expected<void, std::string> check_uuid(const std::string& uuid) const override;

    // TODO: Add methods to check certain sets of fields, for convenience:
    virtual std::expected<void, std::vector<std::string>> check_global(const Global& global) const override;
    virtual std::expected<void, std::vector<std::string>> check_capture(const Capture& capture) const override;
    virtual std::expected<void, std::vector<std::string>> check_annotation(const Annotation& annotation) const override;

    // validate a json object, assuming it is structured like a SigMF metadata file.
    virtual std::expected<void, std::vector<std::string>> check_metadata(const Metadata& meta) const override;

};

} // end v1_2_6 namespace

} // end sigmf_io namespace

#endif // SIGMF_IO_SPEC_VALIDATOR_H
