#ifndef SIGMF_IO_SPEC_VALIDATOR_FACTORY_H
#define SIGMF_IO_SPEC_VALIDATOR_FACTORY_H

#include <string>
#include <memory>

#include "sigmf_io/spec_validator_base.h"
#include "sigmf_io/v1_2_6/spec_validator.h"

namespace sigmf_io {

class SpecValidatorBase;

std::shared_ptr<const SpecValidatorBase> make_spec_validator(const std::string& version);
std::shared_ptr<const SpecValidatorBase> default_spec_validator();

} // namespace sigmf_io

#endif // SIGMF_IO_SPEC_VALIDATOR_FACTORY_H
