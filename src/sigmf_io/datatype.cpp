#include "sigmf_io/datatype.h"

#include <string>
#include <stdexcept>

namespace sigmf_io {

// parse the datatype - throw an error if the data type is invalid.
Datatype::Datatype(const std::string& datatype)
{
    // set datatype so we have an instance to the original string.
    this->datatype_ = datatype;

    // Empty string - throw error
    if (datatype.empty())
        throw std::runtime_error("Datatype: empty format string");

    // Parse real/complex prefix
    if (datatype[0] == 'r') {
        this->sample_format_ = SampleFormat::REAL;
    } else if (datatype[0] == 'c') {
        this->sample_format_ = SampleFormat::COMPLEX;
    } else {
        throw std::runtime_error("Datatype: expected 'r' or 'c' at position 0, got: " + datatype);
    }

    // Remaining substring after the real/complex prefix
    std::string rest = datatype.substr(1);

    // Check for byte types first (no endianness suffix)
    // Endianness is not applicable for byte types, but we still need to
    // set the member. Default to LittleEndian as a neutral value.
    if (rest == "i8") {
        this->sampletype_ = Sampletype::BYTE;
        this->primitive_byte_count_ = 1;
        this->endianness_ = Endianness::LITTLE;
        return;
    } else if (rest == "u8") {
        this->sampletype_ = Sampletype::UBYTE;
        this->primitive_byte_count_ = 1;
        this->endianness_ = Endianness::LITTLE;
        return;
    }

    // Parse multi-char type + endianness: e.g. "f32_le", "u16_be", "i32_le"
    // Find the endianness suffix
    if(rest.size() < 3)
        throw std::runtime_error("Datatype: missing or invalid sample type and/or endianness '" + rest + "' in: " + datatype);

    std::string endianness_suffix = rest.substr(rest.size() - 3);
    std::string type_str = rest.substr(0, rest.size() - 3);
    if (endianness_suffix == "_le") {
        this->endianness_ = Endianness::LITTLE;
    } else if (endianness_suffix == "_be") {
        this->endianness_ = Endianness::BIG;
    } else {
        throw std::runtime_error("Datatype: missing or invalid endianness suffix in: " + datatype);
    }

    // Parse the type token
    if (type_str == "f32") {
        this->sampletype_ = Sampletype::FLOAT_32;
        this->primitive_byte_count_ = 4;
    } else if (type_str == "f64") {
        this->sampletype_ = Sampletype::FLOAT_64;
        this->primitive_byte_count_ = 8;
    } else if (type_str == "i16") {
        this->sampletype_ = Sampletype::INT_16;
        this->primitive_byte_count_ = 2;
    } else if (type_str == "i32") {
        this->sampletype_ = Sampletype::INT_32;
        this->primitive_byte_count_ = 4;
    } else if (type_str == "u16") {
        this->sampletype_ = Sampletype::UINT_16;
        this->primitive_byte_count_ = 2;
    } else if (type_str == "u32") {
        this->sampletype_ = Sampletype::UINT_32;
        this->primitive_byte_count_ = 4;
    } else {
        throw std::runtime_error("Datatype: unknown sample type '" + type_str + "' in: " + datatype);
    }
}


int64_t Datatype::primitives_per_sample() const
{
    return (this->sample_format_ == Datatype::SampleFormat::COMPLEX) ? 2 : 1;
}


int64_t Datatype::bytes_per_sample() const
{
    return this->primitive_byte_count_ * primitives_per_sample();
}

} // end sigmf_io namespace
