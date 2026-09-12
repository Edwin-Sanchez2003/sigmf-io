#ifndef SIGMF_IO_METADATA_H
#define SIGMF_IO_METADATA_H

#include <fstream>
#include <filesystem>
#include <string>
#include <utility>

#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

#include "sigmf_io/spec_validator_factory.h"    // default_spec_validator(), make_spec_validator()
#include "sigmf_io/validation_context.h"        // ValidationLevel, ValidationContext
#include "sigmf_io/spec_validator_base.h"
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
    static constexpr std::string META_EXT = ".sigmf-meta";
    static constexpr std::string DATA_EXT = ".sigmf-data";
public:
    Metadata(
        const Global& g = Global(),
        const std::vector<Capture>& caps = {},
        const std::vector<Annotation>& anns = {},
        ValidationContext validation_context = default_validation_context());
    explicit Metadata(const std::string& meta_path, ValidationContext validation_context = default_validation_context());
    explicit Metadata(const jsoncons::json& meta, ValidationContext validation_context = default_validation_context());

    std::string meta_path() const { return this->meta_path_; }
    std::string data_path() const;

    jsoncons::json to_json() const;

    void save(const std::string& file_path, bool overwrite = false);

    bool is_ncd() const; // checks if its a ncd using the metadata.

    // TODO: implement functions to add captures & Annotations -> needed for enforcing strictness for the validators...
    void add_capture(const Capture& capture);
    void add_annotation(const Annotation& annotation);

    // TODO: implement functions to update ValidationLevel -> needs to propagate to Global/Captures/Annotations...
    void set_validation_context(ValidationContext validation_context);
    const ValidationContext& validation_context() const { return this->validation_context_; }

private:
    std::string meta_path_;
    ValidationContext validation_context_;

    // sets the validation context to global, captures, and annotations.
    void propagate_validation_context();

    bool ends_with(const std::string& value, const std::string& ending) const;

    static jsoncons::json load_json(const std::string& meta_path);
};

} // end sigmf_io namespace

#include "sigmf_io/metadata_traits.h"

#endif // SIGMF_IO_METADATA_H
