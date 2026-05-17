#ifndef SIGMFDATALOADER_H
#define SIGMFDATALOADER_H

#include <mio/mmap.hpp>

#include <vector>
#include <string>
#include <cstdint>
#include "SigMFDataType.h"

/*
 * SigMFDataset
 *
 * The SigMFDataset class represents the raw RF data (real or complex) contained in
 * the .sigmf-data file (or raw RF data files ending in not .sigmf-data for Non-Conforming Datasets).
 *
 * This class handles the actual data loading from disk, presenting the loaded RF data as a
 * std::vector<std::complex<double>> to the user of this interface.
 */

class SigMFDataset
{
public:
    // Construct a SigMFDataset object as an interface with the data on disk, with the minimum required information
    // to interpret the data on disk.
    SigMFDataset(std::string datasetPath, SigMFDataType dataType, int64_t offset, int64_t numChannels, int64_t trailingBytes);
    // TODO: Include constructors that use default values for offset, numChannels, and trailingBytes.

    // Retrieves a vector of samples converted to std::complex<double> given a range of samples and a channel.
    std::vector<std::complex<double>> getSamples(int64_t sampleStart, int64_t sampleCount, int64_t channel);

    // Retrieves a vector of samples converted to std::complex<double> given a range of samples and a channel.
    // This version assumes a channel of 1.
    std::vector<std::complex<double>> getSamples(int64_t sampleStart, int64_t sampleCount);

    SigMFDataType getDataType() const { return this->dataType; }

    // Due to mio memory map implementation, we need to avoid copy construction.
    // Non-copyable, movable
    SigMFDataset(const SigMFDataset&)            = delete;
    SigMFDataset& operator=(const SigMFDataset&) = delete;
    SigMFDataset(SigMFDataset&&)                 = default;
    SigMFDataset& operator=(SigMFDataset&&)      = default;

private:
    SigMFDataType dataType;
    mio::mmap_source mmap;
};

#endif // SIGMFDATALOADER_H
