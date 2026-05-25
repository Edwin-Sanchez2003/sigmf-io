#include "SigMFDataType.h"

#include <string>
#include <stdexcept>


// parse the rawDataType - throw an error if the data type is invalid.
SigMFDataType::SigMFDataType(const std::string rawDataType)
{
    // set rawDataType so we have an instance to the original string.
    this->rawDataType = rawDataType;

    // Empty string - throw error
    if (rawDataType.empty())
        throw std::runtime_error("SigMFDataType: empty format string");

    // Parse real/complex prefix
    if (rawDataType[0] == 'r') {
        this->sampleFormat = SampleFormat::REAL;
    } else if (rawDataType[0] == 'c') {
        this->sampleFormat = SampleFormat::COMPLEX;
    } else {
        throw std::runtime_error("SigMFDataType: expected 'r' or 'c' at position 0, got: " + rawDataType);
    }

    // Remaining substring after the real/complex prefix
    std::string rest = rawDataType.substr(1);

    // Check for byte types first (no endianness suffix)
    // Endianness is not applicable for byte types, but we still need to
    // set the member. Default to LittleEndian as a neutral value.
    if (rest == "i8") {
        this->sampleType = SampleType::BYTE;
        this->primitiveByteCount = 1;
        this->endianness = Endianness::LITTLE;
        return;
    } else if (rest == "u8") {
        this->sampleType = SampleType::UBYTE;
        this->primitiveByteCount = 1;
        this->endianness = Endianness::LITTLE;
        return;
    }

    // Parse multi-char type + endianness: e.g. "f32_le", "u16_be", "i32_le"
    // Find the endianness suffix
    if(rest.size() < 3)
        throw std::runtime_error("SigMFDataType: missing or invalid sample type and/or endianness '" + rest + "' in: " + rawDataType);

    std::string endiannessSuffix = rest.substr(rest.size() - 3);
    std::string typeStr = rest.substr(0, rest.size() - 3);
    if (endiannessSuffix == "_le") {
        this->endianness = Endianness::LITTLE;
    } else if (endiannessSuffix == "_be") {
        this->endianness = Endianness::BIG;
    } else {
        throw std::runtime_error("SigMFDataType: missing or invalid endianness suffix in: " + rawDataType);
    }

    // Parse the type token
    if (typeStr == "f32") {
        this->sampleType = SampleType::FLOAT_32;
        this->primitiveByteCount = 4;
    } else if (typeStr == "f64") {
        this->sampleType = SampleType::FLOAT_64;
        this->primitiveByteCount = 8;
    } else if (typeStr == "i16") {
        this->sampleType = SampleType::INT_16;
        this->primitiveByteCount = 2;
    } else if (typeStr == "i32") {
        this->sampleType = SampleType::INT_32;
        this->primitiveByteCount = 4;
    } else if (typeStr == "u16") {
        this->sampleType = SampleType::UINT_16;
        this->primitiveByteCount = 2;
    } else if (typeStr == "u32") {
        this->sampleType = SampleType::UINT_32;
        this->primitiveByteCount = 4;
    } else {
        throw std::runtime_error("SigMFDataType: unknown sample type '" + typeStr + "' in: " + rawDataType);
    }
}


std::string SigMFDataType::getRawDataType() const
{
    return this->rawDataType;
}


SigMFDataType::SampleFormat SigMFDataType::getSampleFormat() const
{
    return this->sampleFormat;
}


SigMFDataType::SampleType SigMFDataType::getSampleType() const
{
    return this->sampleType;
}


SigMFDataType::Endianness SigMFDataType::getEndianness() const
{
    return this->endianness;
}


// gets the number of bytes that the primitive data type corresponds to.
size_t SigMFDataType::getPrimitiveByteCount() const
{
    return this->primitiveByteCount;
}


size_t SigMFDataType::getPrimitivesPerSample() const
{
    return (this->getSampleFormat() == SigMFDataType::SampleFormat::COMPLEX) ? 2 : 1;
}
