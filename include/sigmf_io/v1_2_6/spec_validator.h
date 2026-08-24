#ifndef SIGMF_IO_SPEC_VALIDATOR_H
#define SIGMF_IO_SPEC_VALIDATOR_H

#include <expected>
#include <string>

#include "sigmf_io/spec_validator_base.h"

namespace sigmf_io {

namespace v1_2_6 {

class SpecValidator : public SpecValidatorBase
{
public:
    SpecValidator() : SpecValidatorBase("1.2.6") {}

    // Specification Field Validation Methods - must implement all base class methods.
    std::expected<void, std::string> check_datatype(const std::string& datatype) const override;
    std::expected<void, std::string> check_sample_rate(double sample_rate) const override;
};

} // end v1_2_6 namespace

} // end sigmf_io namespace

#endif // SIGMF_IO_SPEC_VALIDATOR_H
