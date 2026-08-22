#ifndef SIGMF_IO_RECORDING_H
#define SIGMF_IO_RECORDING_H

#include "sigmf_io/dataset.h"
#include "sigmf_io/metadata.h"
#include <optional>
#include <string>
#include <jsoncons/json.hpp>

namespace sigmf_io {

class Recording
{
public:
    //Dataset data;
    //Metadata meta;
public:
    Recording();
    explicit Recording(const std::string& file_path);

    //std::string meta_path() const { return this->meta_path_; }
    //std::string data_path() const { return this->dataset.value().getDatasetPath(); }

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
