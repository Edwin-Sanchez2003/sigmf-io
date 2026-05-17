#include "SigMFDataset.h"

#include <mio/mmap.hpp>

#include <cstdint>
#include <filesystem>
#include <stdexcept>


SigMFDataset::SigMFDataset(std::string datasetPath, SigMFDataType dataType, int64_t offset, int64_t numChannels, int64_t trailingBytes)
    : dataType(dataType)
{
    // validate datasetPath exists.
    if (std::filesystem::exists(datasetPath) == false)
        throw std::runtime_error("SigMFDataset: Dataset file path '" + datasetPath + "' does not exist!");

    // validate offset is greater than or equal to zero.
    if (offset < 0)
        throw std::runtime_error("SigMFDataset: offset value must be greater than or equal to zero. Given offset: '" + std::to_string(offset) + "'.");

    // validate numChannels is greater than or equal to one.
    if (numChannels < 1)
        throw std::runtime_error(
            "SigMFDataset: numChannels value must be greater than or equal to one. Given numChannels: '" + std::to_string(numChannels) + "'.");

    // validate trailingBytes is greater than or equal to zero.
    if (trailingBytes < 0)
        throw std::runtime_error(
            "SigMFDataset: trailingBytes value must be greater than or equal to zero. Given trailingBytes: '" + std::to_string(trailingBytes) + "'.");

    // create a memory map given the dataset path.
    std::error_code errorCode;
    mmap.map(datasetPath, errorCode);
    if (errorCode)
        throw std::runtime_error("Failed to map file: " + errorCode.message());

    // data is a pointer to the first byte of the file in memory.
    const uint8_t* data = reinterpret_cast<const uint8_t*>(mmap.data());
}
