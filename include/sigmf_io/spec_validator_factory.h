#ifndef SIGMF_IO_SPEC_VALIDATOR_FACTORY_H
#define SIGMF_IO_SPEC_VALIDATOR_FACTORY_H

#include <string>
#include <memory>

#include "sigmf_io/spec_validator_base.h"
#include "sigmf_io/v1_2_6/spec_validator.h"

namespace sigmf_io {

inline std::shared_ptr<const SpecValidatorBase> make_spec_validator(const std::string& version) {
    if (version == "1.2.6") return std::make_shared<v1_2_6::SpecValidator>();
    // if (version == "2.0.0") return std::make_shared<v2_0_0::SpecValidator>();
    throw std::runtime_error("Unsupported SigMF version: " + version + ". Consider picking a new version or disabling SigMF spec validation.");
}

inline std::shared_ptr<const SpecValidatorBase> default_spec_validator() {
    return std::make_shared<v1_2_6::SpecValidator>(); // newest implemented version
}

} // namespace sigmf_io

#endif // SIGMF_IO_SPEC_VALIDATOR_FACTORY_H
