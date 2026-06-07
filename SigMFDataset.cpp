#include "SigMFDataset.h"
#include "SigMFDataType.h"

#include <mio/mmap.hpp>

#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include "sigmf.h"

// define SigMFCapture as the object type in a captures array.
using SigMFCapture = sigmf::Capture<sigmf::core::DescrT>;


SigMFDataset::SigMFDataset(std::string datasetPath, SigMFDataType dataType, int64_t numChannels, int64_t trailingBytes)
    : dataType(dataType)
{
    // validate datasetPath exists.
    if (std::filesystem::exists(datasetPath) == false)
        throw std::runtime_error("SigMFDataset: Dataset file path '" + datasetPath + "' does not exist!");
    this->datasetPath = datasetPath;

    // validate numChannels is greater than or equal to one.
    if (numChannels < 1)
        throw std::runtime_error(
            "SigMFDataset: numChannels value must be greater than or equal to one. Given numChannels: '" + std::to_string(numChannels) + "'.");
    this->numChannels = numChannels;

    // validate trailingBytes is greater than or equal to zero.
    if (trailingBytes < 0)
        throw std::runtime_error(
            "SigMFDataset: trailingBytes value must be greater than or equal to zero. Given trailingBytes: '" + std::to_string(trailingBytes) + "'.");
    this->trailingBytes = trailingBytes;

    // create a memory map given the dataset path.
    std::error_code errorCode;
    this->mmap.map(datasetPath, errorCode);
    if (errorCode)
        throw std::runtime_error("Failed to map file: " + errorCode.message());
}


// Retrieves a vector of samples converted to std::complex<double> given a range of samples and a channel.
std::vector<std::complex<double>> SigMFDataset::getSamples(
    std::vector<SigMFCapture>& captures, int64_t sampleStart, int64_t sampleCount, int64_t channel)
{
    // get the channel's size - used later.
    int64_t channelSize = this->size(captures, channel);

    // check that channel is greater than or equal to one, and that it's less than or equal to numChannels.
    if (channel < 1)
        throw std::runtime_error("Channel index must be at least one! Channel index: '" + std::to_string(channel) + "'.");
    else if (channel > this->numChannels)
        throw std::runtime_error("Channel index must be less than or equal to numChannels! numChannels: '" + std::to_string(this->numChannels) + "', Channel index: '" + std::to_string(channel)+ "'.");

    // check that bounds are valid
    if (sampleStart < 0) {
        throw std::out_of_range("Invalid sampleStart: " + std::to_string(sampleStart));
    }

    // if negative, sets sampleCount to entire channel size.
    if (sampleCount < 0) {
        sampleCount = channelSize;
    }

    // NOTE: sampleStart = 0, sampleCount = 0 passes through silently - returns an empty array.

    // TODO: make sure that sampleStart + sampleCount (accounting for the channel as well)
    // is less than or equal to the dataset size (otherwise we will overflow).
    // written this way to avoid integer overflow (sampleCount + sampleStart).
    if (sampleCount > (channelSize - sampleStart)) {
        throw std::out_of_range(
            "Invalid sample range. sampleStart + sampleCount: '" + std::to_string(sampleStart + sampleCount) +
            "', Channel: '" + std::to_string(channel) + "', size: '" + std::to_string(channelSize) + "'.");
    }

    // get the RF data as a std::vector<std::complex<double>>
    switch (this->dataType.getSampleType()) {
        case SigMFDataType::SampleType::FLOAT_32: return loadSamples<float>(sampleStart, sampleCount, channel);
        case SigMFDataType::SampleType::FLOAT_64: return loadSamples<double>(sampleStart, sampleCount, channel);
        case SigMFDataType::SampleType::INT_16:   return loadSamples<int16_t>(sampleStart, sampleCount, channel);
        case SigMFDataType::SampleType::INT_32:   return loadSamples<int32_t>(sampleStart, sampleCount, channel);
        case SigMFDataType::SampleType::UINT_16:  return loadSamples<uint16_t>(sampleStart, sampleCount, channel);
        case SigMFDataType::SampleType::UINT_32:  return loadSamples<uint32_t>(sampleStart, sampleCount, channel);
        case SigMFDataType::SampleType::BYTE:    return loadSamples<int8_t>(sampleStart, sampleCount, channel);
        case SigMFDataType::SampleType::UBYTE:   return loadSamples<uint8_t>(sampleStart, sampleCount, channel);
        default:
            throw std::runtime_error("Unsupported SigMFDataType::SampleType!");
    }
}


// Returns the size of the dataset, given capture header_byte information, the trailing_bytes, and the
// requested channel. captures array can be an empty vector if dataset is contiguous (no header bytes).
int64_t SigMFDataset::size(std::vector<SigMFCapture>& captures, const int64_t channel) const
{
    // check that the given channel is within bounds
    if(
        channel < 1 ||                  // channels are 1-based indexed.
        channel > this->numChannels     // make sure channel exists.
    ) {
        throw std::runtime_error(
            "Channel index is out-of-bounds. channel: '" + std::to_string(channel) + "', numChannels: '" + std::to_string(this->numChannels));
    }

    int64_t nonSampleBytes = 0;

    // add up header bytes across all captures to get total # of header bytes.
    for(SigMFCapture& capture: captures)
    {
        sigmf::core::CaptureT& cap = capture.access<sigmf::core::CaptureT>();
        nonSampleBytes += cap.header_bytes.value_or(0);
    }

    // add trailing_byte count.
    nonSampleBytes += this->trailingBytes;

    // take total dataset size on disk, subtract header_bytes + trailing_bytes, divide by the number of channels.
    int64_t diskSizeBytes  = static_cast<int64_t>(this->mmap.size());
    int64_t bytesPerSample = static_cast<int64_t>(this->dataType.getBytesPerSample());
    int64_t sampleBytes    = diskSizeBytes - nonSampleBytes; // number of bytes that are actually samples on disk.
    int64_t totalSamples   = sampleBytes / bytesPerSample;  //  number of actual samples on disk.

    // "frame" - refers to a single index of samples across all channels
    // (ie. the sum of all of the samples at index N across all channels).
    int64_t totalFrames      = totalSamples / this->numChannels;

    // The last "frame" may be incomplete - some channels may have 1 more than others.
    int64_t remainderSamples = totalSamples % this->numChannels;

    // Channels are 1-indexed; channel <= remainder get one extra sample
    return totalFrames + ((channel <= remainderSamples) ? 1 : 0);
}
