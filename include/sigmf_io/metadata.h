#ifndef SIGMF_IO_METADATA_H
#define SIGMF_IO_METADATA_H

#include <fstream>
#include <filesystem>
#include <string>
#include <utility>

#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

#include "sigmf_io/global.h"
#include "sigmf_io/capture.h"
#include "sigmf_io/annotation.h"

namespace sigmf_io {

class Metadata
{
public:
    Global global;
    std::vector<Capture> captures;
    std::vector<Annotation> annotations;
public:
    Metadata(
        const Global& global = Global(),
        const std::vector<Capture>& captures = {},
        const std::vector<Annotation>& annotations = {}
    );
    explicit Metadata(const std::string& meta_path);
    explicit Metadata(const jsoncons::json& meta);

    std::string meta_path() const { return this->meta_path_; }

    jsoncons::json to_json() const;

    void save(const std::string& file_path, bool overwrite = false);


    // TODO: Need to write useful/helper functions for common actions using metadata?
    bool is_ncd() const; // checks if its a ncd using the metadata.

private:
    static inline constexpr std::string META_EXT = ".sigmf-meta";
    std::string meta_path_;

    bool ends_with(const std::string& value, const std::string& ending) const;
};

} // end sigmf_io namespace

#endif // SIGMF_IO_METADATA_H
