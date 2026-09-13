#include "sigmf_io/metadata.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <expected>
#include <optional>

#include <jsoncons/json.hpp>

#include "sigmf_io/global.h"          // pulls in global_traits.h
#include "sigmf_io/capture.h"         // pulls in capture_traits.h
#include "sigmf_io/annotation.h"      // pulls in annotation_traits.h
#include "sigmf_io/spec_validator_base.h"

namespace sigmf_io {


// Passes validation context set at construction to global, captures, and annotations.
void Metadata::propagate_validation_context() {
    global.set_validation_context(this->validation_context_);
    for (Capture& c : captures)    c.set_validation_context(this->validation_context_);
    for (Annotation& a : annotations) a.set_validation_context(this->validation_context_);
}

void Metadata::set_validation_context(ValidationContext validation_context)
{
    if ((validation_context.level == ValidationLevel::STRICT ||
         validation_context.level == ValidationLevel::LAZY) &&
        !validation_context.validator)
    {
        throw std::invalid_argument(
            "ValidationContext requires a validator when level is STRICT or LAZY");
    }

    this->validation_context_ = std::move(validation_context);
    this->propagate_validation_context();
}

Metadata::Metadata(
    const Global& g,
    const std::vector<Capture>& caps,
    const std::vector<Annotation>& anns,
    ValidationContext validation_context
) : global(g),
    captures(caps),
    annotations(anns)
{
    this->set_validation_context(std::move(validation_context));
    propagate_validation_context();
}


Metadata::Metadata(const jsoncons::json& meta, ValidationContext validation_context)
    : global(meta.get_value_or<sigmf_io::Global>("global", sigmf_io::Global())),
    captures(meta.get_value_or<std::vector<sigmf_io::Capture>>("captures", std::vector<sigmf_io::Capture>{})),
    annotations(meta.get_value_or<std::vector<sigmf_io::Annotation>>("annotations", std::vector<sigmf_io::Annotation>{}))
{
    this->set_validation_context(std::move(validation_context));
    propagate_validation_context();

    // check against schema at load-time, if STRICT mode is enabled.
    if(this->validation_context_.validator && this->validation_context_.level == ValidationLevel::STRICT)
        sigmf_io::SpecValidatorBase::raise_errors(this->validation_context_.validator->check_metadata(*this));
}


Metadata::Metadata(const std::string& meta_path, ValidationContext validation_context)
    : Metadata(Metadata::load_json(meta_path), validation_context)
{
    this->meta_path_ = meta_path;
}

void Metadata::add_capture(const Capture& capture)
{
    Capture c = capture;
    c.set_validation_context(this->validation_context_);

    if (this->validation_context_.level == ValidationLevel::STRICT)
    {
        SpecValidatorBase::raise_errors(
            this->validation_context_.validator->check_capture(c));
    }

    this->captures.push_back(std::move(c));
}

void Metadata::add_annotation(const Annotation& annotation)
{
    Annotation a = annotation;
    a.set_validation_context(this->validation_context_);

    if (this->validation_context_.level == ValidationLevel::STRICT)
    {
        SpecValidatorBase::raise_errors(
            this->validation_context_.validator->check_annotation(a));
    }

    this->annotations.push_back(std::move(a));
}

// get all annotations that completely or partially overlap with the sample rang:  [sample_start, sample_stop)
std::vector<Annotation> Metadata::get_annotations_in_range(int64_t sample_start, int64_t sample_stop)
{
    std::vector<Annotation> anns;

    for (const Annotation& ann : this->annotations)
    {
        const int64_t ann_start = ann.sample_start();
        const int64_t ann_end   = ann_start + ann.sample_count().value_or(0);

        if (ann_start < sample_stop && sample_start < ann_end)
            anns.push_back(ann);
    }
    return anns;
}

// get all captures that completely or partially overlap with the sample range: [sample_start, sample_stop)
std::vector<Capture> Metadata::get_captures_in_range(int64_t sample_start, int64_t sample_stop)
{
    std::vector<Capture> caps;

    for (size_t i = 0; i < this->captures.size(); ++i)
    {
        const Capture& cap = this->captures[i];
        const int64_t cap_start = cap.sample_start();

        // A capture's end is the next capture's start, or "infinity" if it's the last one.
        const bool has_next = (i + 1 < this->captures.size());
        const int64_t cap_end = has_next ? this->captures[i + 1].sample_start()
                                         : std::numeric_limits<int64_t>::max();

        if (cap_start < sample_stop && sample_start < cap_end)
            caps.push_back(cap);
    }

    return caps;
}

// Reports to the user if the metadata indicates that the dataset is non-conforming.
// NOTE: This function is only valid for an on-disk Metadata file.
bool Metadata::is_ncd() const
{
    // To indicate a non-conforming dataset, we can refer to the core:dataset,
    // core:header_bytes, and core:trailing_bytes fields. If core:dataset exists,
    // core:trailing_bytes is non-zero, or core:header_bytes is non-zero for any
    // capture, then the dataset is non-conforming.

    // first, check core:dataset -> most reliable method to determine dataset conformity.
    if(this->global.dataset().has_value())
    {
        // in case a user uses this field but specifies a .sigmf-data file (technically a misuse
        // of the SigMF format), we still want to handle it as if this is conforming.
        // check if data does not have the sigmf-data extension. If no, it's non-conforming.
        if (std::filesystem::path(this->global.dataset().value()).extension() != Metadata::DATA_EXT)
            return true;
    }

    // if the trailing_bytes value is set to a non-zero value, then the dataset is non-conforming.
    if(this->global.trailing_bytes() > 0)
        return true;

    for(const Capture& cap : this->captures)
        // if ANY capture has non-zero header_bytes, the dataset is non-conforming.
        if(cap.header_bytes() > 0)
            return true;

    // no flags indicating non-conformity found in the metadata - SHOULD be a conforming dataset.
    return false;
}


std::string Metadata::data_path() const
{
    std::filesystem::path meta_path(this->meta_path_);

    // if global's dataset field is populated, use the meta_path directory + that.
    if (this->global.dataset().has_value()) {
        return (meta_path.parent_path() / this->global.dataset().value()).string();
    }

    // No core:dataset field -> compliant recording, same base name, .sigmf-data extension.
    if (meta_path.extension() != Metadata::META_EXT) {
        throw std::runtime_error(
            "SigMF metadata file path does not end with " + Metadata::META_EXT + ": '" + this->meta_path_ + "'");
    }
    meta_path.replace_extension(Metadata::DATA_EXT);
    return meta_path.string();
}


jsoncons::json Metadata::load_json(const std::string& meta_path)
{
    std::ifstream is(meta_path);
    if (!is.is_open()) {
        throw std::runtime_error("Failed to open: " + meta_path);
    }
    return jsoncons::json::parse(is);
}


jsoncons::json Metadata::to_json() const
{
    jsoncons::json meta(jsoncons::json_object_arg);
    meta.insert_or_assign("global", this->global);
    meta.insert_or_assign("captures", this->captures);
    meta.insert_or_assign("annotations", this->annotations);
    return meta;
}


void Metadata::save(const std::string& file_path, bool overwrite)
{
    // validate that file_path given ends with ".sigmf-meta".
    if (!this->ends_with(file_path, this->META_EXT)) {
        throw std::runtime_error(
            "Metadata::save: file_path must end with \"" + META_EXT + "\", got: " + file_path);
    }

    // check if file_path already exists - if yes, & overwrite is false, throw an error.
    // we don't want to overwrite good data! a user should have to explicitly overwrite metadata.
    if (std::filesystem::exists(file_path) && !overwrite) {
        throw std::runtime_error(
            "Metadata::save: file already exists and overwrite is false: " + file_path);
    }

    // final validation against schema.
    // TODO: implement inter-field validation as well! This can only really be checked at write-time.
    if(this->validation_context_.level == ValidationLevel::STRICT || this->validation_context_.level == ValidationLevel::LAZY)
        SpecValidatorBase::raise_errors(this->validation_context_.validator->check_metadata(*this));

    // attempt to open the file & write out to disk.
    std::ofstream out_file(file_path);
    if (!out_file.is_open())
        throw std::runtime_error("Metadata::save: could not open file for writing: " + file_path);

    out_file << jsoncons::pretty_print(this->to_json());
}


bool Metadata::ends_with(const std::string& value, const std::string& ending) const
{
    std::string::size_type pos = value.rfind(ending);
    // it ends with the extension only if the extension is found at the end of the string.
    return pos != std::string::npos && pos + ending.size() == value.size();
}

} // end sigmf_io namespace
