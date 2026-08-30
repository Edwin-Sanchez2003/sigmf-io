#include "sigmf_io/recording.h"

#include <string>
#include <fstream>

#include <jsoncons/json.hpp>

#include "sigmf_io/dataset.h"
#include "sigmf_io/datatype.h"

namespace sigmf_io {

// Can't currently construct Dataset w/copy constructor, due to memory mapping internals -> need better solution later...
// Recording::Recording(const Dataset& dataset, const Metadata& metadata)
//     :data(dataset), meta(metadata)
// {}

Recording::Recording(const std::string& meta_path)
    : meta(meta_path),
    data(this->meta.data_path(),
         Datatype(this->meta.global.datatype()),
         this->meta.global.num_channels(),
         this->meta.global.offset(),
         this->meta.global.trailing_bytes())
{}

} // end sigmf_io namespace
