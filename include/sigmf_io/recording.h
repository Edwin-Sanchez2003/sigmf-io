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
    Metadata meta;
    Dataset data;
public:
    // TODO: Support Default init Recording - currently blocked by Dataset implementation, which
    // requires an existing dataset. Default-init Recordings would be for building new recordings...

    //Recording(const Dataset& dataset, const Metadata& metadata);
    explicit Recording(const std::string& file_path);

    std::string meta_path() const { return this->meta.meta_path(); }
    std::string data_path() const { return this->data.data_path(); }

    template <typename OutputT>
    std::vector<OutputT> get_samples(const int64_t sample_start = 0, int64_t sample_count = -1, const int64_t channel = 1);

    template <typename OutputT>
    std::vector<OutputT> get_capture_samples(const int64_t capture_idx, const int64_t channel = 1);

    template <typename OutputT>
    std::vector<OutputT> get_samples(const Annotation& annotation, const int64_t channel = 1);
};

template <typename OutputT>
std::vector<OutputT> Recording::get_samples(const int64_t sample_start, int64_t sample_count, const int64_t channel)
{
    return this->data.get_samples<OutputT>(this->meta.captures, sample_start, sample_count, channel);
}


template <typename OutputT>
std::vector<OutputT> Recording::get_capture_samples(const int64_t capture_idx, const int64_t channel)
{
    // If captures is empty, assume the whole dataset is the capture.
    if(this->meta.captures.empty())
        return this->data.get_samples<OutputT>(this->meta.captures, 0, -1, channel);

    // Find where the capture ends - either where the next capture starts, or the end of the file if it's the last one.
    const Capture& capture = this->meta.captures.at(capture_idx);
    int64_t sample_count;
    if(capture_idx < (this->meta.captures.size() - 1)) { // NOT the last capture
        sample_count = this->meta.captures.at(capture_idx + 1).sample_start() - capture.sample_start();
    } else { // IS the last capture
        sample_count = this->data.size(this->meta.captures, channel) - capture.sample_start();
    }

    return this->data.get_samples<OutputT>(this->meta.captures, capture.sample_start(), sample_count, channel);
}


template <typename OutputT>
std::vector<OutputT> Recording::get_samples(const Annotation& annotation, const int64_t channel)
{
    return this->data.get_samples<OutputT>(this->meta.captures, annotation.sample_start(), annotation.sample_count(), channel);
}


} // end sigmf_io namespace

#endif // SIGMF_IO_RECORDING_H
