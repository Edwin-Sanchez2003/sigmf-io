#include "sigmf_io/metadata.h"
#include <jsoncons/json.hpp>
#include <fstream>
#include <filesystem>
#include <stdexcept>

namespace sigmf_io {

// Metadata::Metadata(const jsoncons::json& meta)
//     :meta_(meta)
// {}


Metadata::Metadata(const std::string& meta_path)
{
    this->meta_path_ = meta_path;
    std::ifstream is(this->meta_path_);
    if (!is.is_open()) {
        throw std::runtime_error("Failed to open: " + this->meta_path_);
    }
    //this->meta_ = jsoncons::json::parse(is);
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

    // TODO: final validation against schema? other things???

    // attempt to open the file & write out to disk.
    std::ofstream out_file(file_path);
    if (!out_file.is_open())
        throw std::runtime_error("Metadata::save: could not open file for writing: " + file_path);
    //out_file << jsoncons::pretty_print(this->meta_);
}


bool Metadata::ends_with(const std::string& value, const std::string& ending) const
{
    std::string::size_type pos = value.rfind(ending);
    // it ends with the extension only if the extension is found at the end of the string.
    return pos != std::string::npos && pos + ending.size() == value.size();
}

} // end sigmf_io namespace
