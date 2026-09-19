#include "sigmf_io/recording.h"

#include <string>
#include <fstream>

#include <jsoncons/json.hpp>

#include "sigmf_io/validation_context.h"        // ValidationLevel, ValidationContext
#include "sigmf_io/dataset.h"
#include "sigmf_io/datatype.h"

namespace sigmf_io {

// Can't currently construct Dataset w/copy constructor, due to memory mapping internals -> need better solution later...
// Recording::Recording(const Dataset& dataset, const Metadata& metadata)
//     :data(dataset), meta(metadata)
// {}

Recording::Recording(const std::string& meta_path, ValidationContext validation_context)
    : Recording(Metadata(meta_path, std::move(validation_context)))
{}

Recording::Recording(Metadata meta)
    : meta(std::move(meta)),
    data(this->meta.data_path(),
         Datatype(this->meta.global.datatype()),
         this->meta.global.num_channels(),
         this->meta.global.offset(),
         this->meta.global.trailing_bytes())
{}

// NOTE: if no captures present, capture_idx is ignored and size of entire dataset is returned for the given channel.
int64_t Recording::get_capture_size(const int64_t capture_idx, const int64_t channel) const
{
    // if the index is negative, throw an error.
    if(capture_idx < 0)
        throw std::out_of_range("capture_idx must be non-negative. capture_idx: " + std::to_string(capture_idx));

    // if the captures array is empty, then there is 1 capture, and it's the size of the entire dataset.
    if(this->meta.captures().empty())
        return this->data.size(this->meta.captures(), channel);

    // Find where the capture ends - either where the next capture starts, or the end of the file if it's the last one.
    const Capture& capture = this->meta.captures().at(capture_idx);
    int64_t sample_count;
    if(capture_idx < (this->meta.captures().size() - 1)) { // NOT the last capture
        sample_count = this->meta.captures().at(capture_idx + 1).sample_start() - capture.sample_start();
    } else { // IS the last capture
        sample_count = this->data.size(this->meta.captures(), channel) - capture.sample_start();
    }

    return sample_count;
}

} // end sigmf_io namespace
