#ifndef SIGMF_IO_RECORDING_H
#define SIGMF_IO_RECORDING_H

#include <optional>
#include <string>

#include <jsoncons/json.hpp>

#include "sigmf_io/dataset.h"
#include "sigmf_io/metadata.h"

namespace sigmf_io {

class Recording
{
public:
    Dataset data;
    Metadata meta;
public:
    // TODO: Support Default init Recording - currently blocked by Dataset implementation, which
    // requires an existing dataset. Default-init Recordings would be for building new recordings...
    Recording(const Dataset& dataset, const Metadata& metadata);
    explicit Recording(const std::string& file_path);

    std::string meta_path() const { return this->meta.meta_path(); }
    std::string data_path() const { return this->data.data_path(); }

    /* TODO: helper functions for accessing data from a recording using the metadata.
     * - get samples given indices
     * - get samples given captures / capture indices
     * - get samples given annotation/annotation indices
     * - iterators over certain types, etc.
    */

private:
    static constexpr std::string META_EXT = ".sigmf-meta";
    static constexpr std::string DATA_EXT = ".sigmf-data";
};

} // end sigmf_io namespace

#endif // SIGMF_IO_RECORDING_H
