#ifndef SIGMFMETADATA_H
#define SIGMFMETADATA_H

#include <fstream>
#include <filesystem>
#include <string>
#include <utility>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>
#include "sigmf_global.h"
#include "sigmf_capture.h"
#include "sigmf_annotation.h"

class SigMFMetadata
{
public:
    SigMFGlobal global;
    std::vector<SigMFCapture> captures;
    std::vector<SigMFAnnotation> annotations;
public:
    SigMFMetadata();
    SigMFMetadata(
        const SigMFGlobal& global = SigMFGlobal(),
        const std::vector<SigMFCapture>& captures = {},
        const std::vector<SigMFAnnotation>& annotations = {}
    );
    explicit SigMFMetadata(const std::string& meta_path);
    explicit SigMFMetadata(const jsoncons::json& meta);

    std::string meta_path() const { return this->meta_path_; }

    void save(const std::string& file_path, bool overwrite = false);

    // TODO: Need to write useful/helper functions for common actions using metadata?

private:
    static inline constexpr std::string META_EXT = ".sigmf-meta";
    std::string meta_path_;

    bool ends_with(const std::string& value, const std::string& ending) const;
};

#endif // SIGMFMETADATA_H
