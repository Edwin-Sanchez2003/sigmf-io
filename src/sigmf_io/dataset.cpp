#include "sigmf_io/dataset.h"
#include "sigmf_io/datatype.h"

#include <mio/mmap.hpp>

#include <cstdint>
#include <cstddef>
#include <optional>
#include <filesystem>
#include <stdexcept>

namespace sigmf_io {

Dataset::Dataset(const std::string& data_path, Datatype datatype, int64_t num_channels, int64_t offset, int64_t trailing_bytes)
    : datatype_(datatype)
{
    // validate data_path exists.
    if (std::filesystem::exists(data_path) == false)
        throw std::runtime_error("Dataset: Dataset file path '" + data_path + "' does not exist!");
    this->data_path_ = data_path;

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
    this->mmap_.map(data_path, error_code);
    if (error_code)
        throw std::runtime_error("Failed to map file: " + error_code.message());
}


Datatype::Endianness Dataset::system_endianness() {
    uint32_t x = 1;
    uint8_t firstByte = *reinterpret_cast<uint8_t*>(&x);

    return (firstByte == 1) ? Datatype::Endianness::LITTLE : Datatype::Endianness::BIG;
}

void Dataset::check_bounds_or_throw(
    const uint8_t* ptr, int64_t length, const uint8_t* file_begin, const uint8_t* file_end, const std::string& context) const
{
    int64_t file_size   = file_end - file_begin;
    int64_t byte_offset = ptr - file_begin; // may be negative if ptr underflowed
    int64_t available   = (ptr >= file_begin && ptr <= file_end)
                            ? (file_end - ptr)
                            : 0;
    bool out_of_bounds = (ptr < file_begin) || (ptr > file_end) || (length > available);

    if (out_of_bounds)
    {
        int64_t overflow_bytes = length - available;
        std::ostringstream oss;
        oss << context
            << ": attempted to read " << length << " byte(s) at file offset "
            << byte_offset << ", but file size is " << file_size
            << " byte(s) (" << available << " byte(s) available at that offset). "
            << "Overflow by " << overflow_bytes << " byte(s).";
        throw std::out_of_range(oss.str());
    }
}


// Read the header bytes from a given capture.
std::vector<std::byte> Dataset::read_header_bytes(const std::vector<Capture>& captures, const int64_t capture_idx) const
{
    // initial check if capture even has header_bytes.
    const Capture& capture = captures.at(capture_idx);
    std::optional<int64_t> header_bytes = capture.header_bytes();

    // if no header bytes value, return empty array
    if (!header_bytes.has_value() || header_bytes.value() == 0)
        return {};

    // get byte ptr to data on disk
    const uint8_t* file_begin = this->mmap_.data();
    const uint8_t* file_end   = file_begin + this->mmap_.size();
    const uint8_t* byte_ptr   = file_begin;

    // Get initial position of data being requested (ie. first byte of requested data; sample_start).
    int64_t sample_start = capture.sample_start();
    int64_t index_offset_samples = (sample_start - this->offset_) * this->num_channels_;
    int64_t index_offset_bytes = index_offset_samples * this->datatype_.bytes_per_sample();
    byte_ptr += index_offset_bytes;

    // Get accumulated header_bytes from previous captures, up to the desired capture's header.
    int64_t accumulated_header_bytes = 0;
    for (int64_t j = 0; j < capture_idx; j++)
        accumulated_header_bytes += captures[j].header_bytes().value_or(0);
    byte_ptr += accumulated_header_bytes;

    // byte_ptr now points at the start of this capture's header in the mapped file.

    // Bounds check: make sure [byte_ptr, byte_ptr + header_bytes) stays within the mapped region.
    std::ostringstream ctx;
    ctx << "Dataset::read_header_bytes(capture_idx=" << capture_idx
        << ", capture.sample_start()=" << sample_start
        << ", header_bytes=" << header_bytes.value()
        << ", accumulated_prior_header_bytes=" << accumulated_header_bytes << ")";
    check_bounds_or_throw(byte_ptr, header_bytes.value(), file_begin, file_end, ctx.str());

    std::vector<std::byte> bytes(static_cast<size_t>(header_bytes.value()));
    std::memcpy(bytes.data(), byte_ptr, static_cast<size_t>(header_bytes.value()));

    return bytes;
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
