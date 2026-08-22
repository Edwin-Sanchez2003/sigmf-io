#ifndef SIGMF_IO_DATATYPE_H
#define SIGMF_IO_DATATYPE_H

/*
 * Datatype
 *
 * A class that acts as an interface between the datatype string and what the
 * program needs to read the data properly from disk.
 */

#include <string>

namespace sigmf_io {

class Datatype
{
// Enumerations to describe datatype.
public:
    // Tells us if primitives are REAL (1 primitive = 1 sample) or
    // COMPLEX (2 primitives = 1 sample).
    enum class SampleFormat {
        REAL,
        COMPLEX
    };

    // Tells us what the primitive is that is written to disk.
    enum class Sampletype {
        FLOAT_32,
        FLOAT_64,
        INT_16,
        INT_32,
        UINT_16,
        UINT_32,
        BYTE,
        UBYTE
    };

    // Tells us the byte order of the data written to disk.
    enum class Endianness {
        LITTLE,
        BIG
    };

// Datatype public interface.
public:
    Datatype(const std::string& datatype); // constructs Datatype from raw string in SigMF meta file.

    std::string to_string() const { return this->datatype_; }
    SampleFormat sample_format() const { return this->sample_format_; }
    Sampletype sampletype() const { return this->sampletype_; }
    Endianness endianness() const { return this->endianness_; }

    // helpers for interpreting data on disk.
    int64_t primitive_byte_count() const { return this->primitive_byte_count_; }     // How many bytes is one primitive.
    int64_t primitives_per_sample() const;    // How many primitives make up a single sample.
    int64_t bytes_per_sample() const;         // How many bytes make up a single sample.

// Hidden member variables.
private:
    std::string datatype_;    // string representing the raw datatype written in the SigMF meta file.

    SampleFormat sample_format_;  // the format of the current dataset instance.
    Sampletype sampletype_;      // the on-disk data type of the current dataset instance.
    Endianness endianness_;      // the byte order of the current dataset instance.

    size_t primitive_byte_count_;  // the number of bytes based on the dataset's primitive type.
};

} // end sigmf_io namespace

#endif // SIGMF_IO_DATATYPE_H
