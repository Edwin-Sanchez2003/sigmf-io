#ifndef SIGMFDATATYPE_H
#define SIGMFDATATYPE_H

/*
 * SigMFDataType
 *
 * A class that acts as an interface between the datatype string and what the
 * program needs to read the data properly from disk.
 */


#include <string>
#include <unordered_map>


class SigMFDataType
{
// SigMFDataType public interface.
public:
    SigMFDataType(std::string rawDataType); // constructs SigMFDataType from raw string in SigMF meta file.

    SampleFormat getSampleFormat();
    SampleType getSampleType();
    Endianness getEndianness();

    // helpers for interpreting data on disk.
    size_t getPrimitiveByteCount();     // How many bytes is one primitive.
    size_t getPrimitivesPerSample();    // How many primitives make up a single sample.

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

// Hidden member variables.
private:
    std::string rawDataType;    // string representing the raw datatype written in the SigMF meta file.

    SampleFormat sampleFormat;  // the format of the current dataset instance.
    SampleType sampleType;      // the on-disk data type of the current dataset instance.
    Endianness endianness;      // the byte order of the current dataset instance.

    // Maps that store constant information about data types.
    static const std::unordered_map<char, SampleFormat> STRING_SAMPLE_FORMAT_MAP = {
        { 'r', SampleFormat::REAL },
        { 'c', SampleFormat::COMPLEX }
    };

    static const std::unordered_map<std::string, SampleType> STRING_SAMPLE_TYPE_MAP = {
        { "f32", SampleType::FLOAT_32 },
        { "f64", SampleType::FLOAT_64 },
        { "i16", SampleType::INT_16 },
        { "i32", SampleType::INT_32 },
        { "u16", SampleType::UINT_16 },
        { "u32", SampleType::UINT_32 },
        { "i8", SampleType::BYTE },
        { "u8", SampleType::UBYTE }
    };

    static const std::unordered_map<std::string, SampleFormat> STRING_ENDIANNESS_MAP = {
        { "_le", Endianness::LITTLE },
        { "_be", Endianness::BIG }
    };
};

#endif // SIGMFDATATYPE_H
