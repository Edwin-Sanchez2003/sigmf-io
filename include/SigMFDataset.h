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

// outside the class, at the top of the header after your includes

template <typename T>
struct inner_type { using type = T; };

template <typename T>
struct inner_type<std::complex<T>> { using type = T; };

template <typename T>
using inner_type_t = typename inner_type<T>::type;

template <typename T>
constexpr bool is_complex_v = false;

template <typename T>
constexpr bool is_complex_v<std::complex<T>> = true;

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

    template <typename T>
    std::complex<T> byteSwap(std::complex<T> val) const {
        return std::complex<T>(byteSwap(val.real()), byteSwap(val.imag()));
    }

    // loads a chunk of file bytes to a vector of type T, given
    // a type T that represents the type used to store samples in the file.
    template<typename T>
    std::vector<T> loadSamples(
        const std::vector<SigMFCapture>& captures, const int64_t sampleStart, const int64_t sampleCount, const int64_t channel) const
    {
        const int64_t primitivesPerSample = this->dataType.getPrimitivesPerSample();
        const int64_t bytesPerFrame       = primitivesPerSample * static_cast<int64_t>(sizeof(T)) * this->numChannels;
        const int64_t bytesPerSample      = primitivesPerSample * static_cast<int64_t>(sizeof(T));
        const int64_t channelByteOffset   = (channel - 1) * bytesPerSample;

        auto ptrAt = [&](int64_t byteOff) -> const T* {
            return reinterpret_cast<const T*>(
                reinterpret_cast<const uint8_t*>(mmap.data()) + byteOff);
        };

        auto sampleByteOffset = [&](int64_t absIdx, int64_t cumulativeHeaderBytesAtCapture) -> int64_t {
            return absIdx * bytesPerFrame + cumulativeHeaderBytesAtCapture + channelByteOffset;
        };

        bool fileIsLE    = (dataType.getEndianness() == SigMFDataType::Endianness::LITTLE);
        bool machineIsLE = ([]() {
            uint16_t x = 1;
            return *reinterpret_cast<uint8_t*>(&x) == 1;
        })();
        auto toNative = [&](T val) -> T {
            if (fileIsLE != machineIsLE) return byteSwap(val);
            return val;
        };

        std::vector<T> out;
        out.reserve(sampleCount);

        // if no captures, treat as flat file with no header bytes
        if (captures.empty())
        {
            const T* offsetPtr = ptrAt(sampleByteOffset(sampleStart, 0));
            for (int64_t i = 0; i < sampleCount; ++i)
                out.emplace_back(toNative(offsetPtr[i * this->numChannels]));
            return out;
        }

        int64_t samplesRead           = 0;
        int64_t cumulativeHeaderBytes = 0;

        for (std::size_t i = 0; i < captures.size() && samplesRead < sampleCount; ++i)
        {
            const sigmf::core::CaptureT& cap =
                const_cast<SigMFCapture&>(captures[i]).access<sigmf::core::CaptureT>();

            const int64_t capSampleStart = static_cast<int64_t>(cap.sample_start.value_or(0));
            const int64_t capHeaderBytes = static_cast<int64_t>(cap.header_bytes.value_or(0));
            const int64_t capSampleEnd   = (i + 1 < captures.size())
                                             ? static_cast<int64_t>(const_cast<SigMFCapture&>(captures[i + 1]).access<sigmf::core::CaptureT>().sample_start.value_or(0))
                                             : INT64_MAX;

            cumulativeHeaderBytes += capHeaderBytes;

            if (capSampleEnd <= sampleStart) continue;
            if (capSampleStart >= sampleStart + sampleCount) break;

            const int64_t segStart = std::max(sampleStart + samplesRead, capSampleStart);
            const int64_t segEnd   = std::min(sampleStart + sampleCount, capSampleEnd);
            const int64_t segCount = segEnd - segStart;

            const T* segPtr = ptrAt(sampleByteOffset(segStart, cumulativeHeaderBytes));

            for (int64_t si = 0; si < segCount; ++si)
                out.emplace_back(toNative(segPtr[si * this->numChannels]));

            samplesRead += segCount;
        }

        return out;
    }

    // converts a vector of type InputT to type OutputT.
    template <typename InputT, typename OutputT>
    std::vector<OutputT> convert(const std::vector<InputT>& in) {
        using InnerOutputT = inner_type_t<OutputT>;
        using InnerInputT  = inner_type_t<InputT>;

        if constexpr (is_complex_v<InputT> && !is_complex_v<OutputT>) {
            // complex -> real: unsupported
            throw std::runtime_error("Conversion from complex to real is not supported.");

        } else if constexpr (!is_complex_v<InputT> && is_complex_v<OutputT>) {
            // real -> complex: I = sample, Q = 0
            std::vector<OutputT> out(in.size());
            std::transform(in.begin(), in.end(), out.begin(), [](const InputT& x) {
                return OutputT(static_cast<InnerOutputT>(x), InnerOutputT{0});
            });
            return out;

        } else if constexpr (is_complex_v<InputT> && is_complex_v<OutputT>) {
            // complex -> complex
            if constexpr (std::is_same_v<InnerInputT, InnerOutputT>) return in;
            std::vector<OutputT> out(in.size());
            std::transform(in.begin(), in.end(), out.begin(), [](const InputT& x) {
                return OutputT(static_cast<InnerOutputT>(x.real()), static_cast<InnerOutputT>(x.imag()));
            });
            return out;

        } else {
            // real -> real
            if constexpr (std::is_same_v<InputT, OutputT>) return in;
            std::vector<OutputT> out(in.size());
            std::transform(in.begin(), in.end(), out.begin(), [](const InputT& x) {
                return static_cast<OutputT>(x);
            });
            return out;
        }
    }

public:
    // Retrieves a vector of samples converted to std::complex<double> given a range of samples and a channel.
    template <typename OutputT>
    std::vector<OutputT> getSamples(
        const std::vector<SigMFCapture>& captures = {}, const int64_t sampleStart = 0, int64_t sampleCount = -1, const int64_t channel = 1)
    {
        // NOTE: sampleStart = 0, sampleCount = 0 passes through silently - returns an empty array.

        // check that channel is greater than or equal to one, and that it's less than or equal to numChannels.
        if (channel < 1)
            throw std::runtime_error("Channel index must be at least one! Channel index: '" + std::to_string(channel) + "'.");
        else if (channel > this->numChannels)
            throw std::runtime_error("Channel index must be less than or equal to numChannels! numChannels: '" +
                                     std::to_string(this->numChannels) + "', Channel index: '" + std::to_string(channel)+ "'.");

        // get the channel's size - used later.
        int64_t channelSize = this->size(captures, channel);

        // check that bounds are valid
        if (sampleStart < 0) {
            throw std::out_of_range("Invalid sampleStart - must be greater than or equal to zero: '" + std::to_string(sampleStart) + "'.");
        } else if (sampleStart >= channelSize) {
            throw std::out_of_range("Invalid sampleStart - must be less than the number of samples in the channel. sampleStart: '" +
                                    std::to_string(sampleStart) + "', channelSize: '" + std::to_string(channelSize) + "'.");
        }

        // if negative, sets sampleCount to all samples in the channel including and after sampleStart.
        if (sampleCount < 0) {
            sampleCount = channelSize - sampleStart;
        }

        // Make sure that sampleStart + sampleCount (accounting for the channel as well)
        // is less than or equal to the dataset size (otherwise we will overflow/ingest trailing_bytes).
        // written this way to avoid integer overflow (sampleCount + sampleStart).
        if (sampleCount > (channelSize - sampleStart)) {
            throw std::out_of_range(
                "Invalid sample range. sampleStart + sampleCount: '" + std::to_string(sampleStart + sampleCount) +
                "', Channel: '" + std::to_string(channel) + "', size: '" + std::to_string(channelSize) + "'.");
        }

        // get the RF data as the data type specified by the user.
        switch (this->dataType.getSampleType()) {
        case SigMFDataType::SampleType::FLOAT_32:
            if (this->dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX)
                return convert<std::complex<float>, OutputT>(loadSamples<std::complex<float>>(captures, sampleStart, sampleCount, channel));
            else
                return convert<float, OutputT>(loadSamples<float>(captures, sampleStart, sampleCount, channel));
        case SigMFDataType::SampleType::FLOAT_64:
            if (this->dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX)
                return convert<std::complex<double>, OutputT>(loadSamples<std::complex<double>>(captures, sampleStart, sampleCount, channel));
            else
                return convert<double, OutputT>(loadSamples<double>(captures, sampleStart, sampleCount, channel));
        case SigMFDataType::SampleType::INT_16:
            if (this->dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX)
                return convert<std::complex<int16_t>, OutputT>(loadSamples<std::complex<int16_t>>(captures, sampleStart, sampleCount, channel));
            else
                return convert<int16_t, OutputT>(loadSamples<int16_t>(captures, sampleStart, sampleCount, channel));
        case SigMFDataType::SampleType::INT_32:
            if (this->dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX)
                return convert<std::complex<int32_t>, OutputT>(loadSamples<std::complex<int32_t>>(captures, sampleStart, sampleCount, channel));
            else
                return convert<int32_t, OutputT>(loadSamples<int32_t>(captures, sampleStart, sampleCount, channel));
        case SigMFDataType::SampleType::UINT_16:
            if (this->dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX)
                return convert<std::complex<uint16_t>, OutputT>(loadSamples<std::complex<uint16_t>>(captures, sampleStart, sampleCount, channel));
            else
                return convert<uint16_t, OutputT>(loadSamples<uint16_t>(captures, sampleStart, sampleCount, channel));
        case SigMFDataType::SampleType::UINT_32:
            if (this->dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX)
                return convert<std::complex<uint32_t>, OutputT>(loadSamples<std::complex<uint32_t>>(captures, sampleStart, sampleCount, channel));
            else
                return convert<uint32_t, OutputT>(loadSamples<uint32_t>(captures, sampleStart, sampleCount, channel));
        case SigMFDataType::SampleType::BYTE:
            if (this->dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX)
                return convert<std::complex<int8_t>, OutputT>(loadSamples<std::complex<int8_t>>(captures, sampleStart, sampleCount, channel));
            else
                return convert<int8_t, OutputT>(loadSamples<int8_t>(captures, sampleStart, sampleCount, channel));
        case SigMFDataType::SampleType::UBYTE:
            if (this->dataType.getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX)
                return convert<std::complex<uint8_t>, OutputT>(loadSamples<std::complex<uint8_t>>(captures, sampleStart, sampleCount, channel));
            else
                return convert<uint8_t, OutputT>(loadSamples<uint8_t>(captures, sampleStart, sampleCount, channel));
        default:
            throw std::runtime_error("Unsupported SigMFDataType::SampleType!");
        }
    }
};

#endif // SIGMFDATALOADER_H
