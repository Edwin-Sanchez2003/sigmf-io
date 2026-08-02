#include "SigMFMetadata.h"
#include <jsoncons/json.hpp>
#include <fstream>
#include <filesystem>
#include <stdexcept>


SigMFMetadata::SigMFMetadata()
{
    this->meta_["global"] = jsoncons::json::object();
    this->meta_["captures"] = jsoncons::json::array();
    this->meta_["annotations"] = jsoncons::json::array();
}


SigMFMetadata::SigMFMetadata(const jsoncons::json& meta)
{
    this->meta_ = meta;
}


SigMFMetadata::SigMFMetadata(const std::string& meta_path)
{
    this->meta_path_ = meta_path;
    std::ifstream is(this->meta_path_);
    if (!is.is_open()) {
        throw std::runtime_error("Failed to open: " + this->meta_path_);
    }
    this->meta_ = jsoncons::json::parse(is);
}


void SigMFMetadata::save(const std::string& file_path, bool overwrite)
{
    // validate that file_path given ends with ".sigmf-meta".
    if (!this->ends_with(file_path, this->META_EXT)) {
        throw std::runtime_error(
            "SigMFMetadata::save: file_path must end with \"" + META_EXT + "\", got: " + file_path);
    }

    // check if file_path already exists - if yes, & overwrite is false, throw an error.
    // we don't want to overwrite good data! a user should have to explicitly overwrite metadata.
    if (std::filesystem::exists(file_path) && !overwrite) {
        throw std::runtime_error(
            "SigMFMetadata::save: file already exists and overwrite is false: " + file_path);
    }

    // attempt to open the file & write out to disk.
    std::ofstream out_file(file_path);
    if (!out_file.is_open())
        throw std::runtime_error("SigMFMetadata::save: could not open file for writing: " + file_path);
    out_file << jsoncons::pretty_print(this->meta_);
}


bool SigMFMetadata::ends_with(const std::string& value, const std::string& ending) const
{
    std::string::size_type pos = value.rfind(ending);
    // it ends with the extension only if the extension is found at the end of the string.
    return pos != std::string::npos && pos + ending.size() == value.size();
}
