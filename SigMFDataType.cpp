#include "SigMFDataType.h"

#include <string>
#include <stdexcept>


// parse the rawDataType - throw an error if the data type is invalid.
SigMFDataType::SigMFDataType(std::string rawDataType)
{
    // first, figure out if it's real or complex
    char sampleFormatKey = rawDataType.at(0);
    auto iter = SigMFDataType::STRING_SAMPLE_FORMAT_MAP.find(sampleFormatKey);
    if (iter != SigMFDataType::STRING_SAMPLE_FORMAT_MAP.end()) {
        this->SampleFormat = iter->second;
    } else {
        throw std::runtime_error("Unknown Sample Format: " + std::string(1, sampleFormatKey));
    }

    // next, handle byte sample types (2 chars)

    // next, handle remaining sample types (3 chars)
}
