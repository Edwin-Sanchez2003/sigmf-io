#include "sigmf_io/dataset.h"
#include "sigmf_io/datatype.h"

#include <mio/mmap.hpp>

#include <cstdint>
#include <filesystem>
#include <stdexcept>

namespace sigmf_io {

Dataset::Dataset(const std::string& dataset_path, Datatype datatype, int64_t num_channels, int64_t trailing_bytes, int64_t offset)
    : datatype_(datatype)
{
    // validate dataset_path exists.
    if (std::filesystem::exists(dataset_path) == false)
        throw std::runtime_error("Dataset: Dataset file path '" + dataset_path + "' does not exist!");
    this->dataset_path_ = dataset_path;

    // validate num_channels is greater than or equal to one.
    if (num_channels < 1)
        throw std::runtime_error(
            "Dataset: num_channels value must be greater than or equal to one. Given num_channels: '" + std::to_string(num_channels) + "'.");
    this->num_channels_ = num_channels;

    // validate trailingBytes is greater than or equal to zero.
    if (trailing_bytes < 0)
        throw std::runtime_error(
            "Dataset: trailingBytes value must be greater than or equal to zero. Given trailing_bytes: '" + std::to_string(trailing_bytes) + "'.");
    this->trailing_bytes_ = trailing_bytes;

    // validate offset is greater than or equal to zero.
    if (offset < 0)
        throw std::runtime_error(
            "Dataset: offset value must be greater than or equal to zero. Given offset: '" + std::to_string(offset) + "'.");
    this->offset_ = offset;

    // create a memory map given the dataset path.
    std::error_code error_code;
    this->mmap_.map(dataset_path, error_code);
    if (error_code)
        throw std::runtime_error("Failed to map file: " + error_code.message());
}


Datatype::Endianness Dataset::system_endianness() {
    uint32_t x = 1;
    uint8_t firstByte = *reinterpret_cast<uint8_t*>(&x);

    return (firstByte == 1) ? Datatype::Endianness::LITTLE : Datatype::Endianness::BIG;
}


// Returns the size of the dataset, given capture header_byte information, the trailing_bytes, and the
// requested channel. captures array can be an empty vector if dataset is contiguous (no header bytes).
int64_t Dataset::size(const std::vector<Capture>& captures, const int64_t channel) const
{
    // check that the given channel is within bounds
    if(
        channel < 1 ||                  // channels are 1-based indexed.
        channel > this->num_channels_     // make sure channel exists.
    ) {
        throw std::runtime_error(
            "Channel index is out-of-bounds. channel: '" + std::to_string(channel) + "', num_channels: '" + std::to_string(this->num_channels_));
    }

    // stores how many bytes are not samples (header/trailing bytes for Non-Conforming Datasets).
    int64_t non_sample_bytes = 0;

    // add up header bytes across all captures to get total # of header bytes.
    for(const Capture& capture: captures)
    {
        non_sample_bytes += capture.header_bytes().value_or(0);
    }

    // add trailing_byte count.
    non_sample_bytes += this->trailing_bytes_;

    // get the total file size, in bytes.
    int64_t disk_size_bytes  = static_cast<int64_t>(this->mmap_.size());

    // guard against mal-formed metadata - non_sample_bytes add up to more bytes than what are on disk.
    if(non_sample_bytes > disk_size_bytes) {
        throw std::runtime_error("Non-sample byte count exceeds file size — metadata may be malformed. non_sample_bytes: '" +
                                 std::to_string(non_sample_bytes) + "', disk_size_bytes: '" + std::to_string(disk_size_bytes) + "'.");
    }

    // take total dataset size on disk, subtract header_bytes + trailing_bytes, divide by the number of channels.
    int64_t bytes_per_sample = static_cast<int64_t>(this->datatype_.bytes_per_sample());
    int64_t sample_bytes    = disk_size_bytes - non_sample_bytes; // number of bytes that are actually samples on disk.
    int64_t total_samples   = sample_bytes / bytes_per_sample;  //  number of actual samples on disk.

    // "frame" - refers to a single index of samples across all channels
    // (ie. the group of all of the samples at index N across all channels).
    int64_t total_frames      = total_samples / this->num_channels_;

    // The last "frame" may be incomplete - some channels may have 1 more than others.
    int64_t remainder_samples = total_samples % this->num_channels_;

    // Channels are 1-indexed; channel <= remainder get one extra sample
    return total_frames + ((channel <= remainder_samples) ? 1 : 0);
}

} // end sigmf_io namespace
