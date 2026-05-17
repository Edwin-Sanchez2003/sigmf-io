#ifndef SIGMFDATALOADER_H
#define SIGMFDATALOADER_H

#include <mio/mmap.hpp>

#include <vector>
#include <string>
#include <complex>
#include <cstdint>
#include <cstring>
#include <algorithm>
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

    // TODO: gets the size of the dataset
    // for this function to exist, you must iterate over every capture in the metadata to identify header_bytes
    // for Non-Conforming SigMF Datasets. Only then can you calculate the total samples in the dataset.
    // int64_t size() const;

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

    // Swaps byte order - necessary when file type endianness does not match
    // the endianness of the machine it's running on.
    template<typename T>
    T byteSwap(T val) const {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
        uint8_t bytes[sizeof(T)];
        std::memcpy(bytes, &val, sizeof(T));
        std::reverse(bytes, bytes + sizeof(T));
        T result;
        std::memcpy(&result, bytes, sizeof(T));
        return result;
    }

    // loads a chunk of file bytes to a vector of complex doubles, given
    // a type T that represents the type used to store samples in the file.
    template<typename T>
    std::vector<std::complex<double>> loadSamples(int64_t sampleStart, int64_t sampleCount, int64_t channel) const
    {
        // first, cast to the file pointer to the type as specified by the template,
        // and offset by sample_idx_start
        // data is a pointer to the first byte of the file in memory.
        const T* offsetPtr = reinterpret_cast<const T*>(mmap.data()) + (sampleStart * this->dataType.getPrimitivesPerSample());

        // Handle endianness - swap bytes if file is big-endian and machine is
        // little-endian, or if file is little-endian and machine is big-endian
        bool fileIsLE = (dataType.getEndianness() == SigMFDataType::Endianness::LITTLE);
        bool machineIsLE = ([]() {
            uint16_t x = 1;
            return *reinterpret_cast<uint8_t*>(&x) == 1;
        })();

        auto toNative = [&](T val) -> T {
            if (fileIsLE != machineIsLE)
                return byteSwap(val);
            return val;
        };

        // initialize return vector of sample data
        std::vector<std::complex<double>> out;

        // Handle Complex & Real Data Types
        if (dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX) {
            out.reserve(sampleCount);

            // Fast path: cf64_le on a little-endian machine — file layout is already
            // identical to std::complex<double>, so skip the conversion loop entirely.
            if constexpr (std::is_same_v<T, double>) {
                if (fileIsLE && machineIsLE)
                {
                    out.resize(sampleCount);
                    std::memcpy(out.data(), offsetPtr, sampleCount * sizeof(std::complex<double>));
                    return out;
                }
            }

            // General path: convert element by element
            for (int64_t i = 0; i < sampleCount; ++i) {
                double I = static_cast<double>(toNative(offsetPtr[i * 2]));
                double Q = static_cast<double>(toNative(offsetPtr[i * 2 + 1]));
                out.emplace_back(I, Q);
            }
        } else { // Real Numbers
            out.reserve(sampleCount);
            for (int64_t i = 0; i < sampleCount; ++i) {
                double I = static_cast<double>(toNative(offsetPtr[i]));
                out.emplace_back(I, 0.0);
            }
        }

        return out;
    }
};

#endif // SIGMFDATALOADER_H
