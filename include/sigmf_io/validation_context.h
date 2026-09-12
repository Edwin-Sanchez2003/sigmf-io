#ifndef SIGMF_IO_VALIDATION_CONTEXT_H
#define SIGMF_IO_VALIDATION_CONTEXT_H

#include <memory>

namespace sigmf_io {

class SpecValidatorBase;  // forward declaration - include creates circular dependency.

// Defines levels of validation
enum class ValidationLevel { STRICT, LAZY, NONE };

struct ValidationContext {
    std::shared_ptr<const SpecValidatorBase> validator = nullptr;
    ValidationLevel level = ValidationLevel::NONE;
};

ValidationContext default_validation_context();

} // end sigmf_io namespace

#endif // SIGMF_IO_VALIDATION_CONTEXT_H
