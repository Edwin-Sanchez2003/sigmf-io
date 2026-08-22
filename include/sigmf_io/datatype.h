#ifndef DATATYPE_H
#define DATATYPE_H

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
    enum class SampleType {
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
    Datatype(std::string rawDataType); // constructs Datatype from raw string in SigMF meta file.

    std::string to_string() const;
    SampleFormat getSampleFormat() const;
    SampleType getSampleType() const;
    Endianness getEndianness() const;

    // helpers for interpreting data on disk.
    int64_t getPrimitiveByteCount() const;     // How many bytes is one primitive.
    int64_t getPrimitivesPerSample() const;    // How many primitives make up a single sample.
    int64_t getBytesPerSample() const;         // How many bytes make up a single sample.

// Hidden member variables.
private:
    std::string rawDataType;    // string representing the raw datatype written in the SigMF meta file.

    SampleFormat sampleFormat;  // the format of the current dataset instance.
    SampleType sampleType;      // the on-disk data type of the current dataset instance.
    Endianness endianness;      // the byte order of the current dataset instance.

    size_t primitiveByteCount;  // the number of bytes based on the dataset's primitive type.
};

} // end sigmf_io namespace

#endif // DATATYPE_H
