#include "sigmf_io/validation_context.h"

#include <memory>

#include "sigmf_io/v1_2_6/spec_validator.h"

namespace sigmf_io {

ValidationContext default_validation_context()
{
    return ValidationContext(std::make_shared<v1_2_6::SpecValidator>(), ValidationLevel::STRICT);
}

} // end sigmf_io namespace
