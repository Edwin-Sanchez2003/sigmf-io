#ifndef SIGMFRECORDING_H
#define SIGMFRECORDING_H

#include "SigMFDataset.h"
#include <optional>
#include <string>
#include <jsoncons/json.hpp>

class SigMFRecording
{
public:
    std::optional<SigMFDataset> dataset;

public:
    explicit SigMFRecording();
    SigMFRecording(std::string file_path);

    std::string meta_path() const { return this->meta_path_; }
    std::string data_path() const { return this->dataset.value().getDatasetPath(); }

private:
    static constexpr std::string META_EXT = ".sigmf-meta";
    static constexpr std::string DATA_EXT = ".sigmf-data";
    std::string meta_path_;
    jsoncons::json meta_;

    bool endsWith(const std::string& value, const std::string& ending) const;
};

#endif // SIGMFRECORDING_H
