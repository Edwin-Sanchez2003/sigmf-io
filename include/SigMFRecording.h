#ifndef SIGMFRECORDING_H
#define SIGMFRECORDING_H

#include "SigMFDataset.h"
#include "SigMFMetadata.h"
#include <optional>
#include <string>
#include <jsoncons/json.hpp>

class SigMFRecording
{
public:
    SigMFDataset data;
    SigMFMetadata meta;
public:
    SigMFRecording();
    explicit SigMFRecording(const std::string& file_path);

    std::string meta_path() const { return this->meta_path_; }
    std::string data_path() const { return this->dataset.value().getDatasetPath(); }

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

#endif // SIGMFRECORDING_H
