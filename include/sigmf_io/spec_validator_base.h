#ifndef SIGMF_IO_SPEC_VALIDATOR_BASE_H
#define SIGMF_IO_SPEC_VALIDATOR_BASE_H

#include <string>
#include <expected>

#include <jsoncons/json.hpp>

#include "sigmf_io/global.h"
#include "sigmf_io/capture.h"
#include "sigmf_io/annotation.h"

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

    // TODO: Add methods to check certain sets of fields, for convenience:
    virtual std::expected<void, std::string> check_global(const Global& global) const = 0;
    virtual std::expected<void, std::string> check_global(const jsoncons::json& global) const = 0;
    virtual std::expected<void, std::string> check_capture(const Capture& capture) const = 0;
    virtual std::expected<void, std::string> check_capture(const jsoncons::json& capture) const = 0;
    virtual std::expected<void, std::string> check_annotation(const Annotation& annotation) const = 0;
    virtual std::expected<void, std::string> check_annotation(const jsoncons::json& annotation) const = 0;

    // validate a json object, assuming it is structured like a SigMF metadata file.
    virtual std::expected<void, std::string> check_global(const jsoncons::json& meta) const = 0;
private:
    const std::string version_;
};

} // end sigmf_io namespace

#endif // SIGMF_IO_SPEC_VALIDATOR_BASE_H
