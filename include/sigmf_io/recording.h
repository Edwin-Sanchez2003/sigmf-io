#ifndef SIGMF_IO_RECORDING_H
#define SIGMF_IO_RECORDING_H

#include <optional>
#include <string>

#include <jsoncons/json.hpp>

#include "sigmf_io/validation_context.h"        // ValidationLevel, ValidationContext
#include "sigmf_io/capture.h"
#include "sigmf_io/annotation.h"
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
    explicit Recording(const std::string& meta_path, ValidationContext validation_context = default_validation_context());
    explicit Recording(Metadata meta);

    std::string meta_path() const { return this->meta.meta_path(); }
    std::string data_path() const { return this->data.data_path(); }

    // given a capture index, get the size of that capture.
    int64_t get_capture_size(const int64_t capture_idx, const int64_t channel = 1) const;

    template <typename OutputT>
    std::vector<OutputT> get_samples(const int64_t sample_start = 0, int64_t sample_count = -1, const int64_t channel = 1) const;

    template <typename OutputT>
    std::vector<OutputT> get_samples(const Annotation& annotation, const int64_t channel = 1) const;

    template <typename OutputT>
    std::vector<OutputT> get_capture_samples(const int64_t capture_idx, const int64_t channel = 1) const;
};

template <typename OutputT>
std::vector<OutputT> Recording::get_samples(const int64_t sample_start, int64_t sample_count, const int64_t channel) const
{
    return this->data.get_samples<OutputT>(this->meta.captures(), sample_start, sample_count, channel);
}


template <typename OutputT>
std::vector<OutputT> Recording::get_capture_samples(const int64_t capture_idx, const int64_t channel) const
{
    // If captures is empty, assume the whole dataset is the capture.
    if(this->meta.captures().empty())
        return this->data.get_samples<OutputT>(this->meta.captures(), 0, -1, channel);

    // get the capture's bounds.
    const Capture& capture = this->meta.captures().at(capture_idx);
    const int64_t sample_count = this->get_capture_size(capture_idx, channel);

    return this->data.get_samples<OutputT>(this->meta.captures(), capture.sample_start(), sample_count, channel);
}


template <typename OutputT>
std::vector<OutputT> Recording::get_samples(const Annotation& annotation, const int64_t channel) const
{
    // if annotation doesn't have sample_count, then sample_count is whatever
    // value gets us to the end of the capture this annotation belongs to:
    int64_t sample_count = 0;
    if (annotation.sample_count().has_value()) {
        sample_count = annotation.sample_count().value();
    } else {
        // identify the capture this annotation lives in.
        int64_t cap_idx = this->meta.find_capture_containing(annotation.sample_start());
        int64_t cap_size  = this->get_capture_size(cap_idx);
        const Capture& cap = this->meta.captures().at(cap_idx);

        // compute the size of the annotation as ann.sample_start to the end of the capture.
        sample_count = cap.sample_start() + cap_size - annotation.sample_start();
    }

    return this->data.get_samples<OutputT>(this->meta.captures(), annotation.sample_start(), sample_count, channel);
}


} // end sigmf_io namespace

#endif // SIGMF_IO_RECORDING_H
