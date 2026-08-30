#include "sigmf_io/metadata.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <expected>

#include <jsoncons/json.hpp>

#include "sigmf_io/global.h"
#include "sigmf_io/capture.h"
#include "sigmf_io/annotation.h"
#include "sigmf_io/v1_2_6/spec_validator.h"

namespace sigmf_io {


Metadata::Metadata(const std::string& meta_path)
    : Metadata(Metadata::load_json(meta_path))
{
    this->meta_path_ = meta_path;
}


Metadata::Metadata(const jsoncons::json& meta)
{
    this->global = Global(meta["global"]);
    this->captures.clear();
    // NOTE: These can be converted to jsoncons::json type traits to define how to directly convert to/from jsoncons array types.
    this->captures.reserve(meta["captures"].size());
    for (const auto& capture : meta["captures"].array_range())
        this->captures.emplace_back(capture); // calls constructor in-place w/jsoncons::json object.
    this->annotations.clear();
    this->annotations.reserve(meta["annotations"].size());
    for (const auto& annotation : meta["annotations"].array_range())
        this->annotations.emplace_back(annotation); // calls constructor in-place w/jsoncons::json object.

    // check against schema at load-time
    sigmf_io::v1_2_6::SpecValidator spec_validator;
    sigmf_io::v1_2_6::SpecValidator::raise_errors(spec_validator.check_metadata(*this));
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
    meta.insert_or_assign("global", this->global.to_json());

    jsoncons::json captures_arr(jsoncons::json_array_arg);
    for (const Capture& capture : this->captures) {
        captures_arr.push_back(capture.to_json());
    }
    meta.insert_or_assign("captures", captures_arr);

    jsoncons::json annotations_arr(jsoncons::json_array_arg);
    for (const Annotation& annotation : this->annotations) {
        annotations_arr.push_back(annotation.to_json());
    }
    meta.insert_or_assign("annotations", annotations_arr);

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

    //  final validation against schema? other things???
    sigmf_io::v1_2_6::SpecValidator spec_validator;
    sigmf_io::v1_2_6::SpecValidator::raise_errors(spec_validator.check_metadata(*this));

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
