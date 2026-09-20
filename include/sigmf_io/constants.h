#ifndef SIGMF_IO_FIELD_CONSTANTS_H
#define SIGMF_IO_FIELD_CONSTANTS_H

#include <string>

/*
 * Names constants within the SigMF Specification for user convenience within
 * the sigmf_io namespace.
 *
 * NOTES:
 * - Break into global, capture, annotation, etc. namespaces?
 * - Put into a 'core' namespace within C++?
 */

namespace sigmf_io {

namespace constants {

// core namespace - prepended to every field within SigMF's core namespace.
constexpr std::string_view CORE = "core:";
constexpr std::string_view GLOBAL = "global";
constexpr std::string_view CAPTURES = "captures";
constexpr std::string_view ANNOTATIONS = "annotations";

// Global fields
constexpr std::string_view DATATYPE = "core:datatype";
constexpr std::string_view SAMPLE_RATE = "core:sample_rate";
constexpr std::string_view AUTHOR = "core:author";
constexpr std::string_view COLLECTION = "core:collection";
constexpr std::string_view DATASET = "core:dataset";
constexpr std::string_view DATA_DOI = "core:data_doi";
constexpr std::string_view DESCRIPTION = "core:description";
constexpr std::string_view HW = "core:hw";
constexpr std::string_view LICENSE = "core:license";
constexpr std::string_view METADATA_ONLY = "core:metadata_only";
constexpr std::string_view META_DOI = "core:meta_doi";
constexpr std::string_view NUM_CHANNELS = "core:num_channels";
constexpr std::string_view OFFSET = "core:offset";
constexpr std::string_view RECORDER = "core:recorder";
constexpr std::string_view SHA512 = "core:sha512";
constexpr std::string_view TRAILING_BYTES = "core:trailing_bytes";
constexpr std::string_view VERSION = "core:version";
constexpr std::string_view GEOLOCATION = "core:geolocation";
constexpr std::string_view EXTENSIONS = "core:extensions";

// Capture fields
constexpr std::string_view SAMPLE_START = "core:sample_start";
constexpr std::string_view DATETIME = "core:datetime";
constexpr std::string_view FREQUENCY = "core:frequency";
constexpr std::string_view GLOBAL_INDEX = "core:global_index";
constexpr std::string_view HEADER_BYTES = "core:header_bytes";

// Annotation fields
constexpr std::string_view SAMPLE_COUNT = "core:sample_count";
constexpr std::string_view FREQ_LOWER_EDGE = "core:freq_lower_edge";
constexpr std::string_view FREQ_UPPER_EDGE = "core:freq_upper_edge";
constexpr std::string_view LABEL = "core:label";
constexpr std::string_view COMMENT = "core:comment";
constexpr std::string_view GENERATOR = "core:generator";
constexpr std::string_view UUID = "core:uuid";

} // end constants namespace (within sigmf_io)

inline std::string pointer(std::string_view key)
{
    std::string p;
    p.reserve(key.size() + 1);
    p += '/';
    p += key;
    return p;
}

} // end sigmf_io namespace

#endif // SIGMF_IO_FIELD_CONSTANTS_H
