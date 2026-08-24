#ifndef SIGMF_IO_SPEC_VALIDATOR_BASE_H
#define SIGMF_IO_SPEC_VALIDATOR_BASE_H

#include <string>
#include <expected>

/*
 * SpecValidatorBase
 *
 * This class defines the interface that is used to validate fields within SigMF Metadata.
 * This class itself cannot be instantiated; It is meant to be a pure virtual interface that
 * child classes inherit from and implement based on a specific version of the SigMF Specification.
 */

namespace sigmf_io {

class SpecValidatorBase
{
public:
    explicit SpecValidatorBase(const std::string& version):version_(version) {}
    virtual ~SpecValidatorBase() = default;

    std::string version() const { return this->version_; }

    // Specification Field Validation Methods - to be implemented by derived classes.
    virtual std::expected<void, std::string> check_datatype(const std::string& datatype) const = 0;
    virtual std::expected<void, std::string> check_sample_rate(double sample_rate) const = 0;
    // TODO: Add in the rest of the known core fields...
private:
    const std::string version_;
};

} // end sigmf_io namespace

#endif // SIGMF_IO_SPEC_VALIDATOR_BASE_H
