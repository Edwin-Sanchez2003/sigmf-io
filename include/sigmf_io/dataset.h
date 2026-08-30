#ifndef SIGMF_IO_DATASET_H
#define SIGMF_IO_DATASET_H

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <complex>
#include <algorithm>

#include <mio/mmap.hpp>

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
    Dataset(const std::string& data_path, Datatype datatype, int64_t num_channels = 1, int64_t offset = 0, int64_t trailing_bytes = 0);

    /* Getters & Setters */
    std::string data_path() const { return this->data_path_; }
    Datatype datatype() const { return this->datatype_; }
    int64_t num_channels() const { return this->num_channels_; }
    int64_t trailing_bytes() const { return this->trailing_bytes_; }
    int64_t offset() const { return this->offset_; }
    static Datatype::Endianness system_endianness();

    // Retrieves a vector of samples converted to std::complex<double> given a range of samples and a channel.
    template <typename OutputT>
    std::vector<OutputT> get_samples(
        const std::vector<Capture>& captures = {}, const int64_t sample_start = 0, int64_t sample_count = -1, const int64_t channel = 1);

    // NOTE: Should I add a method to get multiple channels at the same time (ie. just give the user a contiguous block of samples,
    // across all channels? In case it's too slow to ask for each channel one-at-a-time, since they're interleaved?

    // Returns the number of samples in the dataset.
    // Must factor in header_bytes, footer_bytes, and channel.
    int64_t size(const std::vector<Capture>& captures = {}, const int64_t channel = 1) const;

    // Read the header bytes from a given capture.
    std::vector<std::byte> read_header_bytes(const Capture& capture) const;

    // Due to mio memory map implementation, we need to avoid copy construction.
    // Non-copyable, movable
    Dataset(const Dataset&)            = delete;
    Dataset& operator=(const Dataset&) = delete;
    Dataset(Dataset&&)                 = default;
    Dataset& operator=(Dataset&&)      = default;

private:
    std::string data_path_;      // Path to the dataset file to be interacted with on disk.
    Datatype datatype_;             // The SigMF Datatype; tells us how to read data from disk.
    int64_t num_channels_;          // The number of interleaved streams of samples in this Dataset.
    int64_t trailing_bytes_;        // The number of bytes at the end of the file that are NOT samples (NCDs).
    int64_t offset_;                // A logical offset applied to all sample-related indices in some SigMFRecordings.
    mio::basic_mmap<mio::access_mode::read, uint8_t> mmap_;      // The Memory Mapping instance used to read data from disk at runtime.

private:
    // Swaps byte order - necessary when file type endianness does not match
    // the endianness of the machine it's running on.
    template<typename T>
    T byte_swap(T val) const;

    template <typename T>
    std::complex<T> byte_swap(std::complex<T> val) const;

    // Get the value at a certain byte, converted to the targeted data type.
    // This reads the byte as a certain data type.
    template<typename T>
    T value_at(const uint8_t* byte_ptr) const;

    // loads a chunk of file bytes to a vector of type T, given
    // a type T that represents the type used to store samples in the file.
    template<typename T>
    std::vector<T> load_samples(
        const std::vector<Capture>& captures, const int64_t sample_start, const int64_t sample_count, const int64_t channel) const;

    // converts a vector of type InputT to type OutputT.
    template <typename InputT, typename OutputT>
    std::vector<OutputT> convert(const std::vector<InputT>& in);
};


template<typename T>
T Dataset::byte_swap(T val) const {
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
    uint8_t bytes[sizeof(T)];
    std::memcpy(bytes, &val, sizeof(T));
    std::reverse(bytes, bytes + sizeof(T));
    T result;
    std::memcpy(&result, bytes, sizeof(T));
    return result;
}


template<typename T>
std::complex<T> Dataset::byte_swap(std::complex<T> val) const {
    return std::complex<T>(byte_swap(val.real()), byte_swap(val.imag()));
}


template<typename T>
T Dataset::value_at(const uint8_t* byte_ptr) const {
    const T* primitive_ptr = reinterpret_cast<const T*>(byte_ptr);
    return *primitive_ptr;
}


template<typename T>
std::vector<T> Dataset::load_samples(
    const std::vector<Capture>& captures, const int64_t sample_start, const int64_t sample_count, const int64_t channel) const
{
    // NOTE: Assumes that the global value core:offset is implicitly added to all captures/annotations/sample indices!!!

    // get the pointer to the first byte in our file.
    const uint8_t* byte_ptr = this->mmap_.data();

    // allocate a vector of sample_count of type T.
    std::vector<T> out;
    out.reserve(sample_count); // pre-allocate the memory needed for this vector, without initializing the values (cheap).

    // Get initial position of data being requested (ie. first byte of requested data; sample_start).
    int64_t index_offset_samples = (sample_start - this->offset_) * this->num_channels_ + (channel - 1);
    int64_t index_offset_bytes = index_offset_samples * this->datatype_.bytes_per_sample();
    byte_ptr += index_offset_bytes;

    // Get accumulated header_bytes from captures, up to sample_start.
    int64_t accumulated_header_bytes = 0;
    int64_t capture_idx = 0;
    for (; capture_idx < captures.size(); capture_idx++)
    {
        const Capture& cap = captures[capture_idx];

        // check if the capture start sample index comes before sample_start meaning
        // we need to apply header bytes offset.
        if ((cap.sample_start() - this->offset_) <= sample_start)
        {
            accumulated_header_bytes += cap.header_bytes().value_or(0);
        } else {
            break;
        }
    }
    byte_ptr += accumulated_header_bytes;

    // Loop to aggregate samples into out vector
    for (int64_t i = 0; i < sample_count; i++)
    {
        // 1. If we pass a capture boundary, offset by header_bytes again.
        while (capture_idx < captures.size()) {
            const Capture& cap = captures[capture_idx];
            if ((cap.sample_start() - this->offset_) <= (sample_start + i))
            {
                byte_ptr += cap.header_bytes().value_or(0);
                capture_idx++;
            } else {
                break;
            }
        }

        // 2. cast current sentry (initially index_offset_bytes) to on-disk primitive type.
        T sample = this->value_at<T>(byte_ptr);

        // 3. Perform endianness swap if necessary & push back into vector.
        if (this->datatype_.endianness() != Dataset::system_endianness()) {
            out.push_back(this->byte_swap(sample));
        } else {
            out.push_back(sample);
        }

        // 4. Increment by formula: `bytes_per_sample * num_channels`
        byte_ptr += this->datatype_.bytes_per_sample() * this->num_channels_;

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
std::vector<OutputT> Dataset::get_samples(
    const std::vector<Capture>& captures, const int64_t sample_start, int64_t sample_count, const int64_t channel)
{
    // NOTE: sample_start = 0, sample_count = 0 passes through silently - returns an empty array.

    // check that channel is greater than or equal to one, and that it's less than or equal to numChannels.
    if (channel < 1)
        throw std::runtime_error("Channel index must be at least one! Channel index: '" + std::to_string(channel) + "'.");
    else if (channel > this->num_channels_)
        throw std::runtime_error("Channel index must be less than or equal to numChannels! numChannels: '" +
                                 std::to_string(this->num_channels_) + "', Channel index: '" + std::to_string(channel)+ "'.");

    // get the channel's size - used later.
    int64_t samples_in_channel = this->size(captures, channel);

    // check that bounds are valid
    if (sample_start < 0) {
        throw std::out_of_range("Invalid sample_start - must be greater than or equal to zero: '" + std::to_string(sample_start) + "'.");
    } else if (sample_start >= samples_in_channel) {
        throw std::out_of_range("Invalid sample_start - must be less than the number of samples in the channel. sample_start: '" +
                                std::to_string(sample_start) + "', samples_in_channel: '" + std::to_string(samples_in_channel) + "'.");
    }

    // if negative, sets sample_count to all samples in the channel including and after sample_start.
    if (sample_count < 0) {
        sample_count = samples_in_channel - sample_start;
    }

    // Make sure that sample_start + sample_count (accounting for the channel as well)
    // is less than or equal to the dataset size (otherwise we will overflow/ingest trailing_bytes).
    // written this way to avoid integer overflow (sample_count + sample_start).
    if (sample_count > (samples_in_channel - sample_start)) {
        throw std::out_of_range(
            "Invalid sample range. sample_start + sample_count: '" + std::to_string(sample_start + sample_count) +
            "', Channel: '" + std::to_string(channel) + "', size: '" + std::to_string(samples_in_channel) + "'.");
    }

    // get the RF data as the data type specified by the user.
    switch (this->datatype_.sampletype()) {
    case Datatype::Sampletype::FLOAT_32:
        if (this->datatype_.sample_format() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<float>, OutputT>(load_samples<std::complex<float>>(captures, sample_start, sample_count, channel));
        else
            return convert<float, OutputT>(load_samples<float>(captures, sample_start, sample_count, channel));
    case Datatype::Sampletype::FLOAT_64:
        if (this->datatype_.sample_format() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<double>, OutputT>(load_samples<std::complex<double>>(captures, sample_start, sample_count, channel));
        else
            return convert<double, OutputT>(load_samples<double>(captures, sample_start, sample_count, channel));
    case Datatype::Sampletype::INT_16:
        if (this->datatype_.sample_format() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<int16_t>, OutputT>(load_samples<std::complex<int16_t>>(captures, sample_start, sample_count, channel));
        else
            return convert<int16_t, OutputT>(load_samples<int16_t>(captures, sample_start, sample_count, channel));
    case Datatype::Sampletype::INT_32:
        if (this->datatype_.sample_format() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<int32_t>, OutputT>(load_samples<std::complex<int32_t>>(captures, sample_start, sample_count, channel));
        else
            return convert<int32_t, OutputT>(load_samples<int32_t>(captures, sample_start, sample_count, channel));
    case Datatype::Sampletype::UINT_16:
        if (this->datatype_.sample_format() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<uint16_t>, OutputT>(load_samples<std::complex<uint16_t>>(captures, sample_start, sample_count, channel));
        else
            return convert<uint16_t, OutputT>(load_samples<uint16_t>(captures, sample_start, sample_count, channel));
    case Datatype::Sampletype::UINT_32:
        if (this->datatype_.sample_format() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<uint32_t>, OutputT>(load_samples<std::complex<uint32_t>>(captures, sample_start, sample_count, channel));
        else
            return convert<uint32_t, OutputT>(load_samples<uint32_t>(captures, sample_start, sample_count, channel));
    case Datatype::Sampletype::BYTE:
        if (this->datatype_.sample_format() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<int8_t>, OutputT>(load_samples<std::complex<int8_t>>(captures, sample_start, sample_count, channel));
        else
            return convert<int8_t, OutputT>(load_samples<int8_t>(captures, sample_start, sample_count, channel));
    case Datatype::Sampletype::UBYTE:
        if (this->datatype_.sample_format() == Datatype::SampleFormat::COMPLEX)
            return convert<std::complex<uint8_t>, OutputT>(load_samples<std::complex<uint8_t>>(captures, sample_start, sample_count, channel));
        else
            return convert<uint8_t, OutputT>(load_samples<uint8_t>(captures, sample_start, sample_count, channel));
    default:
        throw std::runtime_error("Unsupported Datatype::Sampletype!");
    }
}

} // end sigmf_io namespace

#endif // SIGMF_IO_DATASET_H
