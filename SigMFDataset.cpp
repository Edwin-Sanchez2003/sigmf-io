#include "SigMFDataset.h"
#include "SigMFDataType.h"

#include <mio/mmap.hpp>

#include <cstdint>
#include <filesystem>
#include <stdexcept>


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
    int64_t sampleStart, int64_t sampleCount, int64_t channel)
{
    // get the channel's size - used later.
    int64_t channelSize = this->size(channel);

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
// requested channel.captures array can be an empty vector if dataset is contiguous (no header bytes).
int64_t SigMFDataset::size(std::vector<SigMFCapture> captures, int64_t channel = 1) const
{
    // add up header bytes across all captures to get total # of header bytes.

    // add trailing_byte count.

    // take total dataset size on disk, subtract header_bytes + trailing_bytes, divide by the number of channels.

}
