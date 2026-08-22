#include "sigmf_io/dataset.h"
#include "sigmf_io/datatype.h"

#include <mio/mmap.hpp>

#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include "sigmf.h"

namespace sigmf_io {

Dataset::Dataset(std::string datasetPath, Datatype dataType, int64_t numChannels, int64_t trailingBytes, int64_t offset)
    : dataType(dataType)
{
    // validate datasetPath exists.
    if (std::filesystem::exists(datasetPath) == false)
        throw std::runtime_error("Dataset: Dataset file path '" + datasetPath + "' does not exist!");
    this->datasetPath = datasetPath;

    // validate numChannels is greater than or equal to one.
    if (numChannels < 1)
        throw std::runtime_error(
            "Dataset: numChannels value must be greater than or equal to one. Given numChannels: '" + std::to_string(numChannels) + "'.");
    this->numChannels = numChannels;

    // validate trailingBytes is greater than or equal to zero.
    if (trailingBytes < 0)
        throw std::runtime_error(
            "Dataset: trailingBytes value must be greater than or equal to zero. Given trailingBytes: '" + std::to_string(trailingBytes) + "'.");
    this->trailingBytes = trailingBytes;

    // validate offset is greater than or equal to zero.
    if (offset < 0)
        throw std::runtime_error(
            "Dataset: offset value must be greater than or equal to zero. Given offset: '" + std::to_string(offset) + "'.");
    this->offset = offset;

    // create a memory map given the dataset path.
    std::error_code errorCode;
    this->mmap.map(datasetPath, errorCode);
    if (errorCode)
        throw std::runtime_error("Failed to map file: " + errorCode.message());
}


Datatype::Endianness Dataset::getSystemEndianness() const {
    uint32_t x = 1;
    uint8_t firstByte = *reinterpret_cast<uint8_t*>(&x);

    return (firstByte == 1) ? Datatype::Endianness::LITTLE : Datatype::Endianness::BIG;
}


// Returns the size of the dataset, given capture header_byte information, the trailing_bytes, and the
// requested channel. captures array can be an empty vector if dataset is contiguous (no header bytes).
int64_t Dataset::size(const std::vector<SigMFCapture>& captures, const int64_t channel) const
{
    // check that the given channel is within bounds
    if(
        channel < 1 ||                  // channels are 1-based indexed.
        channel > this->numChannels     // make sure channel exists.
    ) {
        throw std::runtime_error(
            "Channel index is out-of-bounds. channel: '" + std::to_string(channel) + "', numChannels: '" + std::to_string(this->numChannels));
    }

    // stores how many bytes are not samples (header/trailing bytes for Non-Conforming Datasets).
    int64_t nonSampleBytes = 0;

    // add up header bytes across all captures to get total # of header bytes.
    for(const SigMFCapture& capture: captures)
    {
        // const_cast -> stupid hack to allow for const function arguments, which allows for default empty vector...
        const sigmf::core::CaptureT& cap = const_cast<SigMFCapture&>(capture).access<sigmf::core::CaptureT>();
        nonSampleBytes += cap.header_bytes.value_or(0);
    }

    // add trailing_byte count.
    nonSampleBytes += this->trailingBytes;

    // get the total file size, in bytes.
    int64_t diskSizeBytes  = static_cast<int64_t>(this->mmap.size());

    // guard against mal-formed metadata - nonSampleBytes add up to more bytes than what are on disk.
    if(nonSampleBytes > diskSizeBytes) {
        throw std::runtime_error("Non-sample byte count exceeds file size — metadata may be malformed. nonSampleBytes: '" +
                                 std::to_string(nonSampleBytes) + "', diskSizeBytes: '" + std::to_string(diskSizeBytes) + "'.");
    }

    // take total dataset size on disk, subtract header_bytes + trailing_bytes, divide by the number of channels.
    int64_t bytesPerSample = static_cast<int64_t>(this->dataType.getBytesPerSample());
    int64_t sampleBytes    = diskSizeBytes - nonSampleBytes; // number of bytes that are actually samples on disk.
    int64_t totalSamples   = sampleBytes / bytesPerSample;  //  number of actual samples on disk.

    // "frame" - refers to a single index of samples across all channels
    // (ie. the group of all of the samples at index N across all channels).
    int64_t totalFrames      = totalSamples / this->numChannels;

    // The last "frame" may be incomplete - some channels may have 1 more than others.
    int64_t remainderSamples = totalSamples % this->numChannels;

    // Channels are 1-indexed; channel <= remainder get one extra sample
    return totalFrames + ((channel <= remainderSamples) ? 1 : 0);
}

} // end sigmf_io namespace
