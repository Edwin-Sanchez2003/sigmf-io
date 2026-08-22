#ifndef SIGMF_IO_DATASET_H
#define SIGMF_IO_DATASET_H

#include <mio/mmap.hpp>

#include <vector>
#include <string>
#include <complex>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include "sigmf_io/datatype.h"
#include "sigmf_io/capture.h"

namespace sigmf_io {

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
 * Dataset
 *
 * The Dataset class represents the raw RF data (real or complex) contained in
 * the .sigmf-data file (or raw RF data files ending in not .sigmf-data for Non-Conforming Datasets).
 *
 * This class handles the actual data loading from disk, presenting the loaded RF data as a
 * std::vector<std::complex<double>> to the user of this interface.
 */

class Dataset
{
public:
    // Construct a Dataset object as an interface with the data on disk, with the minimum required information
    // to interpret the data on disk.
    Dataset(std::string datasetPath, Datatype dataType, int64_t numChannels = 1, int64_t trailingBytes = 0, int64_t offset = 0);

    /* Getters & Setters */
    std::string getDatasetPath() const { return this->datasetPath; }
    Datatype getDataType() const { return this->dataType; }
    int64_t getNumChannels() const { return this->numChannels; }
    int64_t getTrailingBytes() const { return this->trailingBytes; }
    int64_t getOffset() const { return this->offset; }
    Datatype::Endianness getSystemEndianness() const;

    // Retrieves a vector of samples converted to std::complex<double> given a range of samples and a channel.
    template <typename OutputT>
    std::vector<OutputT> getSamples(
        const std::vector<Capture>& captures = {}, const int64_t sampleStart = 0, int64_t sampleCount = -1, const int64_t channel = 1);

    // Returns the number of samples in the dataset.
    // Must factor in header_bytes, footer_bytes, and channel.
    int64_t size(const std::vector<Capture>& captures = {}, const int64_t channel = 1) const;

    // Due to mio memory map implementation, we need to avoid copy construction.
    // Non-copyable, movable
    Dataset(const Dataset&)            = delete;
    Dataset& operator=(const Dataset&) = delete;
    Dataset(Dataset&&)                 = default;
    Dataset& operator=(Dataset&&)      = default;

private:
    std::string datasetPath;    // Path to the dataset file to be interacted with on disk.
    Datatype dataType;     // The SigMF Datatype; tells us how to read data from disk.
    int64_t numChannels;        // The number of interleaved streams of samples in this Dataset.
    int64_t trailingBytes;      // The number of bytes at the end of the file that are NOT samples (NCDs).
    int64_t offset;             // A logical offset applied to all sample-related indices in some SigMFRecordings.
    mio::basic_mmap<mio::access_mode::read, uint8_t> mmap;      // The Memory Mapping instance used to read data from disk at runtime.

private:
    // Swaps byte order - necessary when file type endianness does not match
    // the endianness of the machine it's running on.
    template<typename T>
    T byteSwap(T val) const;

    template <typename T>
    std::complex<T> byteSwap(std::complex<T> val) const;

    // Get the value at a certain byte, converted to the targeted data type.
    // This reads the byte as a certain data type.
    template<typename T>
    T valueAt(const uint8_t* bytePtr) const;

    // loads a chunk of file bytes to a vector of type T, given
    // a type T that represents the type used to store samples in the file.
    template<typename T>
    std::vector<T> loadSamples(
        const std::vector<Capture>& captures, const int64_t sampleStart, const int64_t sampleCount, const int64_t channel) const;

    // converts a vector of type InputT to type OutputT.
    template <typename InputT, typename OutputT>
    std::vector<OutputT> convert(const std::vector<InputT>& in);
};


template<typename T>
T Dataset::byteSwap(T val) const {
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
    uint8_t bytes[sizeof(T)];
    std::memcpy(bytes, &val, sizeof(T));
    std::reverse(bytes, bytes + sizeof(T));
    T result;
    std::memcpy(&result, bytes, sizeof(T));
    return result;
}


template<typename T>
std::complex<T> Dataset::byteSwap(std::complex<T> val) const {
    return std::complex<T>(byteSwap(val.real()), byteSwap(val.imag()));
}


template<typename T>
T Dataset::valueAt(const uint8_t* bytePtr) const {
    const T* primitivePtr = reinterpret_cast<const T*>(bytePtr);
    return *primitivePtr;
}


template<typename T>
std::vector<T> Dataset::loadSamples(
    const std::vector<Capture>& captures, const int64_t sampleStart, const int64_t sampleCount, const int64_t channel) const
{
    // NOTE: Assumes that the global value core:offset is implicitly added to all captures/annotations/sample indices!!!

    // get the pointer to the first byte in our file.
    const uint8_t* bytePtr = mmap.data();

    // allocate a vector of sampleCount of type T.
    std::vector<T> out;
    out.reserve(sampleCount); // pre-allocate the memory needed for this vector, without initializing the values (cheap).

    // Get initial position of data being requested (ie. first byte of requested data; sampleStart).
    int64_t indexOffsetSamples = (sampleStart - this->offset) * this->numChannels + (channel - 1);
    int64_t indexOffsetBytes = indexOffsetSamples * this->getDataType().getBytesPerSample();
    bytePtr += indexOffsetBytes;

    // Get accumulated header_bytes from captures, up to sampleStart.
    int64_t accumulatedHeaderBytes = 0;
    int64_t captureIdx = 0;
    for (; captureIdx < captures.size(); captureIdx++)
    {
        const Capture& cap = captures[captureIdx];

        // check if the capture start sample index comes before sampleStart meaning
        // we need to apply header bytes offset.
        if ((cap.sample_start() - this->offset) <= sampleStart)
        {
            accumulatedHeaderBytes += cap.header_bytes().value_or(0);
        } else {
            break;
        }
    }
    bytePtr += accumulatedHeaderBytes;

    // Loop to aggregate samples into out vector
    for (int64_t i = 0; i < sampleCount; i++)
    {
        // 1. If we pass a capture boundary, offset by header_bytes again.
        while (captureIdx < captures.size()) {
            const Capture& cap = captures[captureIdx];
            if ((cap.sample_start() - this->offset) <= (sampleStart + i))
            {
                bytePtr += cap.header_bytes().value_or(0);
                captureIdx++;
            } else {
                break;
            }
        }

        // 2. cast current sentry (initially index_offset_bytes) to on-disk primitive type.
        T sample = this->valueAt<T>(bytePtr);

        // 3. Perform endianness swap, if necessary & push back into vector.
        if (this->dataType.getEndianness() != this->getSystemEndianness()) {
            out.push_back(this->byteSwap(sample));
        } else {
            out.push_back(sample);
        }

        // 4. Increment by formula: `bytes_per_sample * num_channels`
        bytePtr += this->dataType.getBytesPerSample() * this->numChannels;

    } // end aggregate samples

    return out;
}


template <typename InputT, typename OutputT>
std::vector<OutputT> Dataset::convert(const std::vector<InputT>& in) {
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


template <typename OutputT>
std::vector<OutputT> Dataset::getSamples(
    const std::vector<Capture>& captures, const int64_t sampleStart, int64_t sampleCount, const int64_t channel)
{
    // NOTE: sampleStart = 0, sampleCount = 0 passes through silently - returns an empty array.

    // check that channel is greater than or equal to one, and that it's less than or equal to numChannels.
    if (channel < 1)
        throw std::runtime_error("Channel index must be at least one! Channel index: '" + std::to_string(channel) + "'.");
    else if (channel > this->numChannels)
        throw std::runtime_error("Channel index must be less than or equal to numChannels! numChannels: '" +
                                 std::to_string(this->numChannels) + "', Channel index: '" + std::to_string(channel)+ "'.");

    // get the channel's size - used later.
    int64_t samplesInChannel = this->size(captures, channel);

    // check that bounds are valid
    if (sampleStart < 0) {
        throw std::out_of_range("Invalid sampleStart - must be greater than or equal to zero: '" + std::to_string(sampleStart) + "'.");
    } else if (sampleStart >= samplesInChannel) {
        throw std::out_of_range("Invalid sampleStart - must be less than the number of samples in the channel. sampleStart: '" +
                                std::to_string(sampleStart) + "', samplesInChannel: '" + std::to_string(samplesInChannel) + "'.");
    }

    // if negative, sets sampleCount to all samples in the channel including and after sampleStart.
    if (sampleCount < 0) {
        sampleCount = samplesInChannel - sampleStart;
    }

    // Make sure that sampleStart + sampleCount (accounting for the channel as well)
    // is less than or equal to the dataset size (otherwise we will overflow/ingest trailing_bytes).
    // written this way to avoid integer overflow (sampleCount + sampleStart).
    if (sampleCount > (samplesInChannel - sampleStart)) {
        throw std::out_of_range(
            "Invalid sample range. sampleStart + sampleCount: '" + std::to_string(sampleStart + sampleCount) +
            "', Channel: '" + std::to_string(channel) + "', size: '" + std::to_string(samplesInChannel) + "'.");
    }

    // get the RF data as the data type specified by the user.
    switch (this->dataType.getSampleType()) {
    case Datatype::SampleType::FLOAT_32:
        if (this->dataType.getSampleFormat() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<float>, OutputT>(loadSamples<std::complex<float>>(captures, sampleStart, sampleCount, channel));
        else
            return convert<float, OutputT>(loadSamples<float>(captures, sampleStart, sampleCount, channel));
    case Datatype::SampleType::FLOAT_64:
        if (this->dataType.getSampleFormat() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<double>, OutputT>(loadSamples<std::complex<double>>(captures, sampleStart, sampleCount, channel));
        else
            return convert<double, OutputT>(loadSamples<double>(captures, sampleStart, sampleCount, channel));
    case Datatype::SampleType::INT_16:
        if (this->dataType.getSampleFormat() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<int16_t>, OutputT>(loadSamples<std::complex<int16_t>>(captures, sampleStart, sampleCount, channel));
        else
            return convert<int16_t, OutputT>(loadSamples<int16_t>(captures, sampleStart, sampleCount, channel));
    case Datatype::SampleType::INT_32:
        if (this->dataType.getSampleFormat() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<int32_t>, OutputT>(loadSamples<std::complex<int32_t>>(captures, sampleStart, sampleCount, channel));
        else
            return convert<int32_t, OutputT>(loadSamples<int32_t>(captures, sampleStart, sampleCount, channel));
    case Datatype::SampleType::UINT_16:
        if (this->dataType.getSampleFormat() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<uint16_t>, OutputT>(loadSamples<std::complex<uint16_t>>(captures, sampleStart, sampleCount, channel));
        else
            return convert<uint16_t, OutputT>(loadSamples<uint16_t>(captures, sampleStart, sampleCount, channel));
    case Datatype::SampleType::UINT_32:
        if (this->dataType.getSampleFormat() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<uint32_t>, OutputT>(loadSamples<std::complex<uint32_t>>(captures, sampleStart, sampleCount, channel));
        else
            return convert<uint32_t, OutputT>(loadSamples<uint32_t>(captures, sampleStart, sampleCount, channel));
    case Datatype::SampleType::BYTE:
        if (this->dataType.getSampleFormat() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<int8_t>, OutputT>(loadSamples<std::complex<int8_t>>(captures, sampleStart, sampleCount, channel));
        else
            return convert<int8_t, OutputT>(loadSamples<int8_t>(captures, sampleStart, sampleCount, channel));
    case Datatype::SampleType::UBYTE:
        if (this->dataType.getSampleFormat() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<uint8_t>, OutputT>(loadSamples<std::complex<uint8_t>>(captures, sampleStart, sampleCount, channel));
        else
            return convert<uint8_t, OutputT>(loadSamples<uint8_t>(captures, sampleStart, sampleCount, channel));
    default:
        throw std::runtime_error("Unsupported Datatype::SampleType!");
    }
}

} // end sigmf_io namespace

#endif // SIGMF_IO_DATASET_H
