# SigMFDataset
The SigMF Dataset class is meant to interface with the file system to load signal data into memory.

## Data Loading
A SigMFDataset refers to the file on disk that contains signal data. This, along with a SigMFMetadata file, is used to load signal data from disk into memory.

To load the signal data, you need to know the following parameters from the metadata:

### Global
From the global field, you need:
* **core:datatype** - this field gives you information on how individual samples are stored on disk in a given SigMFDataset file.
    - primitive data type on disk (byte, int, float, etc.)
    - endianness of the data (byte order - big/little endian).
    - sample type (whether the data is complex or real-valued signal data).
* **core:num_channels** - Some data collection systems have multiple sources of data streams (channels). As they are recorded in real-time,  this data is interleaved when written to disk. This field tells you the number of channels that are interleaved in the given SigMFDataset file.
* **core:trailing_bytes** - This field refers to SigMFDataset files that have data that is not signal data appended to the end of the file. This field tells you how many bytes to ignore at the end of the file.
* **core:offset** - this field (used in SigMFCollections) is used to record the relative position of the file within a stream of recorded SigMFDataset files. **core:sample_start** values within a SigMFDataset must be greater than or equal to this value. Therefore, to read data off of the disk, you must subtract this value from each core:sample_start value within a capture or annotation to get the actual location of the sample on disk.
---
### Captures/Annotations
* **core:header_bytes** - this field tells you how many bytes at the beginning of a capture contains data that is not signal data. When reading data from disk, you must aggregate all of the header_bytes from the current capture with all previous captures to get the real sample position of the data on disk.
---
### User-Provided Information
To read any data, the user must first provide the following information:
* **channel** - Since there can be multiple, interleaved, independent channels (ie. data streams) in the same file, the user must indicate which channel they wish to load when requesting to load a chunk of signal data.
* **sample_start** - This indicates which locigal sample you want to start at when loading data on disk. It must be between 0 and the total number of samples on disk, given a specific channel.
* **sample_count** - This indicates how many samples to load, starting at *sample_start*. This number must be less than or equal to the remaining number of samples available on disk after the sample_start value.
---
### Putting It All Together - Algorithm
To load data from disk, we must combine all of this information to identify the real sample location on disk:

```
sum_header_bytes = 0
for capture in captures:
    if capture.sample_start > sample_start:
        break
    sum_header_bytes += capture.header_bytes

frame_size_bytes = num_channels * components_per_sample * primitive_byte_count

index_offset = (sample_start - offset) * num_channels + (channel - 1)
index_offset_bytes = index_offset * components_per_sample * primitive_byte_count

index_physical_start_byte = sum_header_bytes + index_offset_bytes
```

### Notes
* You would then read up to `sample_count` samples from disk (accounting for the `data_type`), starting at and including `index_physical_start_byte`, making sure to offset by `num_channels`, and to additionally accumulating `header_bytes` when crossing between two captures.
* Make sure to do a byte order swap when the endianness of the data on disk is different from that of the computer you're running on.
* If there are more than 1 channel, then you can't read data as a contiguous block. You must offset by frame_size_bytes as you accumulate signal data across a channel.
* Be sure to not read past the size of the dataset channel. The actual size of the dataset on disk can be calculated by taking the size of the file in bytes, subtracting the header_bytes of each capture, the trailing_bytes, dividing by the number of bytes per sample (primitive byte count times 2 if complex, 1 otherwise), dividing by the total number of channels, and then adding the remainder of samples for cases when the last frame is incomplete:

```cpp
// Returns the size of the dataset, given capture header_byte information, the trailing_bytes, and the
// requested channel. captures array can be an empty vector if dataset is contiguous (no header bytes).
int64_t SigMFDataset::size(const std::vector<SigMFCapture>& captures, const int64_t channel) const
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

    // take total dataset size on disk, subtract header_bytes + trailing_bytes, divide by the number of channels.
    int64_t diskSizeBytes  = static_cast<int64_t>(this->mmap.size());
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
```
