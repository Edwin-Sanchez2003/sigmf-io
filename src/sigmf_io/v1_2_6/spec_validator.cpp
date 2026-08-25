#include "sigmf_io/v1_2_6/spec_validator.h"

namespace sigmf_io {

namespace v1_2_6 {

// TODO: begin implementing v1.2.6 specification checking...


std::expected<void, std::string> check_datatype(const std::string& datatype) const
{

}

std::expected<void, std::string> check_sample_rate(double sample_rate) const {}
std::expected<void, std::string> check_author(const std::string& author) const {}
std::expected<void, std::string> check_collection(const std::string& collection) const {}
std::expected<void, std::string> check_dataset(const std::string& dataset) const {}
std::expected<void, std::string> check_data_doi(const std::string& data_doi) const {}
std::expected<void, std::string> check_description(const std::string& description) const {}
std::expected<void, std::string> check_hw(const std::string& hw) const {}
std::expected<void, std::string> check_license(const std::string& license) const {}
std::expected<void, std::string> check_metadata_only(bool metadata_only) const {}
std::expected<void, std::string> check_meta_doi(const std::string& meta_doi) const {}
std::expected<void, std::string> check_num_channels(int64_t num_channels) const {}
std::expected<void, std::string> check_offset(int64_t offset) const {}
std::expected<void, std::string> check_recorder(const std::string& recorder) const {}
std::expected<void, std::string> check_sha512(const std::string& sha512) const {}
std::expected<void, std::string> check_trailing_bytes(int64_t trailing_bytes) const {}
std::expected<void, std::string> check_version(const std::string& version) const {}
// Check Geolocation

// Capture checks
std::expected<void, std::string> check_sample_start(int64_t sample_start) const {}
std::expected<void, std::string> check_datetime(const std::string& datetime) const {}
std::expected<void, std::string> check_frequency(double frequency) const {}
std::expected<void, std::string> check_global_index(int64_t global_index) const {}
std::expected<void, std::string> check_header_bytes(int64_t header_bytes) const {}
// Check Geolocation

// Annotation checks
std::expected<void, std::string> check_sample_count(int64_t sample_count) const {}
std::expected<void, std::string> check_freq_lower_edge(double freq_lower_edge) const {}
std::expected<void, std::string> check_freq_upper_edge(double freq_upper_edge) const {}
std::expected<void, std::string> check_label(const std::string& label) const {}
std::expected<void, std::string> check_comment(const std::string& comment) const {}
std::expected<void, std::string> check_generator(const std::string& generator) const {}
std::expected<void, std::string> check_uuid(const std::string& uuid) const {}

// TODO: Add methods to check certain sets of fields, for convenience:
std::expected<void, std::string> check_global(const Global& global) const {}
std::expected<void, std::string> check_global(const jsoncons::json& global) const {}
std::expected<void, std::string> check_capture(const Capture& capture) const {}
std::expected<void, std::string> check_capture(const jsoncons::json& capture) const {}
std::expected<void, std::string> check_annotation(const Annotation& annotation) const {}
std::expected<void, std::string> check_annotation(const jsoncons::json& annotation) const {}

// validate a json object, assuming it is structured like a SigMF metadata file.
std::expected<void, std::string> check_metadata(const jsoncons::json& meta) const {}

} // end v1_2_6 namespace

} // end sigmf_io namespace
