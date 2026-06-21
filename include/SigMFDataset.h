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
#include "sigmf.h"

using SigMFCapture = sigmf::Capture<sigmf::core::DescrT>;

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
    SigMFDataset(std::string datasetPath, SigMFDataType dataType, int64_t numChannels = 1, int64_t trailingBytes = 0);

    // Retrieves a vector of samples converted to std::complex<double> given a range of samples and a channel.
    // This is convenient when your dataset is contiguous samples (ie. there are no header_bytes in any captures).
    std::vector<std::complex<double>> getSamples(
        const std::vector<SigMFCapture>& captures = {}, const int64_t sampleStart = 0, int64_t sampleCount = -1, const int64_t channel = 1);

    /* Getters & Setters */
    std::string getDatasetPath() const { return this->datasetPath; }
    SigMFDataType getDataType() const { return this->dataType; }
    int64_t getNumChannels() const { return this->numChannels; }
    int64_t getTrailingBytes() const { return this->trailingBytes; }

    // Returns the number of samples in the dataset.
    // Must factor in header_bytes, footer_bytes, and channel.
    int64_t size(const std::vector<SigMFCapture>& captures = {}, const int64_t channel = 1) const;

    // Due to mio memory map implementation, we need to avoid copy construction.
    // Non-copyable, movable
    SigMFDataset(const SigMFDataset&)            = delete;
    SigMFDataset& operator=(const SigMFDataset&) = delete;
    SigMFDataset(SigMFDataset&&)                 = default;
    SigMFDataset& operator=(SigMFDataset&&)      = default;

private:
    std::string datasetPath;    // Path to the dataset file to be interacted with on disk.
    SigMFDataType dataType;     // The SigMF Datatype; tells us how to read data from disk.
    int64_t numChannels;        // The number of interleaved streams of samples in this SigMFDataset.
    int64_t trailingBytes;      // The number of bytes at the end of the file that are NOT samples (NCDs).
    mio::mmap_source mmap;      // The Memory Mapping instance used to read data from disk at runtime.

private:
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
    // TODO: loadSamples needs to handle Non-Conforming Dataset cases!!! (header_bytes).
    // NOTE: This function assumes valid inputs - maybe move input validation logic to this function...
    template<typename T>
    std::vector<std::complex<double>> loadSamples(const std::vector<SigMFCapture>& captures, const int64_t sampleStart, const int64_t sampleCount, const int64_t channel) const
    {
        const int64_t primitivesPerSample = this->dataType.getPrimitivesPerSample();
        const int64_t bytesPerFrame       = primitivesPerSample * static_cast<int64_t>(sizeof(T))
                                      * this->numChannels;
        const int64_t bytesPerSample      = primitivesPerSample * static_cast<int64_t>(sizeof(T));
        const int64_t channelByteOffset   = (channel - 1) * bytesPerSample;

        // Helper: given a byte offset into the mmap, return a typed pointer into it.
        auto ptrAt = [&](int64_t byteOff) -> const T* {
            return reinterpret_cast<const T*>(
                reinterpret_cast<const uint8_t*>(mmap.data()) + byteOff);
        };

        // Helper: given an absolute sample index, return the byte offset of the first
        // primitive of `channel` for that sample, accounting for all header bytes in
        // all captures that precede (or own) that sample.
        // `captureIndex` is the index into `captures` that contains `absIdx`.
        // `cumulativeHeaderBytesAtCapture` is the sum of header_bytes for captures[0..captureIndex].
        auto sampleByteOffset = [&](int64_t absIdx,
                                    int64_t cumulativeHeaderBytesAtCapture) -> int64_t {
            return absIdx * bytesPerFrame + cumulativeHeaderBytesAtCapture + channelByteOffset;
        };

        // Handle endianness.
        bool fileIsLE    = (dataType.getEndianness() == SigMFDataType::Endianness::LITTLE);
        bool machineIsLE = ([]() {
            uint16_t x = 1;
            return *reinterpret_cast<uint8_t*>(&x) == 1;
        })();
        auto toNative = [&](T val) -> T {
            if (fileIsLE != machineIsLE)
                return byteSwap(val);
            return val;
        };

        // Initialize return vector.
        std::vector<std::complex<double>> out;
        out.reserve(sampleCount);

        // -------------------------------------------------------------------------
        // Fast path: no captures metadata => no header bytes anywhere.
        // -------------------------------------------------------------------------
        if (captures.empty())
        {
            const T* offsetPtr = ptrAt(sampleByteOffset(sampleStart, 0));

            if (dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX)
            {
                if constexpr (std::is_same_v<T, double>)
                {
                    if (fileIsLE && machineIsLE && this->numChannels == 1)
                    {
                        out.resize(sampleCount);
                        std::memcpy(out.data(), offsetPtr, sampleCount * sizeof(std::complex<double>));
                        return out;
                    }
                }
                const int64_t stride = 2 * this->numChannels;
                for (int64_t i = 0; i < sampleCount; ++i)
                {
                    double I = static_cast<double>(toNative(offsetPtr[i * stride]));
                    double Q = static_cast<double>(toNative(offsetPtr[i * stride + 1]));
                    out.emplace_back(I, Q);
                }
            }
            else
            {
                for (int64_t i = 0; i < sampleCount; ++i)
                {
                    double I = static_cast<double>(toNative(offsetPtr[i * this->numChannels]));
                    out.emplace_back(I, 0.0);
                }
            }
            return out;
        }

        // -------------------------------------------------------------------------
        // General path: walk captures, reading one segment at a time.
        //
        // Each capture owns samples [capSampleStart, capSampleEnd).
        // Within the file those samples are preceded by capHeaderBytes non-sample bytes.
        // We read only the slice of samples that falls within [sampleStart, sampleStart+sampleCount),
        // advancing `samplesRead` until the request is satisfied.
        // -------------------------------------------------------------------------
        const bool isComplex = (dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX);
        const int64_t stride = 2 * this->numChannels; // only used for complex

        int64_t samplesRead            = 0;
        int64_t cumulativeHeaderBytes  = 0;
        bool    started                = false;

        for (std::size_t i = 0; i < captures.size() && samplesRead < sampleCount; ++i)
        {
            const sigmf::core::CaptureT& cap =
                const_cast<SigMFCapture&>(captures[i]).access<sigmf::core::CaptureT>();

            const int64_t capSampleStart = static_cast<int64_t>(cap.sample_start.value_or(0));
            const int64_t capHeaderBytes = static_cast<int64_t>(cap.header_bytes.value_or(0));

            // Determine the exclusive end of this capture's sample range.
            const int64_t capSampleEnd = (i + 1 < captures.size())
                                             ? static_cast<int64_t>(const_cast<SigMFCapture&>(captures[i + 1]).access<sigmf::core::CaptureT>().sample_start.value_or(0))
                                             : INT64_MAX;

            // Accumulate this capture's header bytes before computing any pointer into it.
            cumulativeHeaderBytes += capHeaderBytes;

            // Skip captures that end before our request begins.
            if (capSampleEnd <= sampleStart)
                continue;

            // Stop if this capture starts after our request ends.
            if (capSampleStart >= sampleStart + sampleCount)
                break;

            started = true;

            // Clamp to the intersection of [sampleStart+samplesRead, sampleStart+sampleCount)
            // and [capSampleStart, capSampleEnd).
            const int64_t segStart = std::max(sampleStart + samplesRead, capSampleStart);
            const int64_t segEnd   = std::min(sampleStart + sampleCount, capSampleEnd);
            const int64_t segCount = segEnd - segStart;

            const T* segPtr = ptrAt(sampleByteOffset(segStart, cumulativeHeaderBytes));

            if (isComplex)
            {
                for (int64_t si = 0; si < segCount; ++si)
                {
                    double I = static_cast<double>(toNative(segPtr[si * stride]));
                    double Q = static_cast<double>(toNative(segPtr[si * stride + 1]));
                    out.emplace_back(I, Q);
                }
            }
            else
            {
                for (int64_t si = 0; si < segCount; ++si)
                {
                    double I = static_cast<double>(toNative(segPtr[si * this->numChannels]));
                    out.emplace_back(I, 0.0);
                }
            }

            samplesRead += segCount;
        }

        // If sampleStart didn't fall in any capture, fall back to no-header behaviour.
        if (!started)
        {
            const T* offsetPtr = ptrAt(sampleByteOffset(sampleStart, 0));
            if (isComplex)
            {
                for (int64_t i = 0; i < sampleCount; ++i)
                {
                    double I = static_cast<double>(toNative(offsetPtr[i * stride]));
                    double Q = static_cast<double>(toNative(offsetPtr[i * stride + 1]));
                    out.emplace_back(I, Q);
                }
            }
            else
            {
                for (int64_t i = 0; i < sampleCount; ++i)
                {
                    double I = static_cast<double>(toNative(offsetPtr[i * this->numChannels]));
                    out.emplace_back(I, 0.0);
                }
            }
        }

        return out;
    }

};

#endif // SIGMFDATALOADER_H
