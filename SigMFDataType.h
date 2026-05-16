#ifndef SIGMFDATATYPE_H
#define SIGMFDATATYPE_H

/*
 * SigMFDataType
 *
 * A class that acts as an interface between the datatype string and what the
 * program needs to read the data properly from disk.
 */


#include <string>


class SigMFDataType
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

// SigMFDataType public interface.
public:
    SigMFDataType(std::string rawDataType); // constructs SigMFDataType from raw string in SigMF meta file.

    SampleFormat getSampleFormat() const;
    SampleType getSampleType() const;
    Endianness getEndianness() const;

    // helpers for interpreting data on disk.
    size_t getPrimitiveByteCount() const;     // How many bytes is one primitive.
    size_t getPrimitivesPerSample() const;    // How many primitives make up a single sample.

// Hidden member variables.
private:
    std::string rawDataType;    // string representing the raw datatype written in the SigMF meta file.

    SampleFormat sampleFormat;  // the format of the current dataset instance.
    SampleType sampleType;      // the on-disk data type of the current dataset instance.
    Endianness endianness;      // the byte order of the current dataset instance.

    size_t primitiveByteCount;  // the number of bytes based on the dataset's primitive type.
};

#endif // SIGMFDATATYPE_H
