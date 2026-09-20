#include "sigmf_io/annotation.h"
#include "sigmf_io/json_base.h"

#include "sigmf_io/spec_validator_base.h"
#include "sigmf_io/validation_context.h"
#include "sigmf_io/constants.h"

namespace sigmf_io {

Annotation::Annotation(const jsoncons::json& data, ValidationContext validation_context)
    : JSONBase(Annotation::default_data(), data, validation_context)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_errors(this->validation_context_.validator->check_annotation(*this));
}


jsoncons::json Annotation::default_data()
{
    jsoncons::json defaults;
    defaults[constants::SAMPLE_START] = 0;
    return defaults;
}


int64_t Annotation::sample_start() const
{
    return this->get<int64_t>(sigmf_io::pointer(constants::SAMPLE_START));
}


void Annotation::set_sample_start(int64_t sample_start)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_sample_start(sample_start));

    this->set(sigmf_io::pointer(constants::SAMPLE_START), sample_start);
}


std::optional<int64_t> Annotation::sample_count() const
{
    return this->get_optional<int64_t>(sigmf_io::pointer(constants::SAMPLE_COUNT));
}


void Annotation::set_sample_count(int64_t sample_count)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_sample_count(sample_count));

    this->set(sigmf_io::pointer(constants::SAMPLE_COUNT), sample_count);
}


std::optional<double> Annotation::freq_lower_edge() const
{
    return this->get_optional<double>(sigmf_io::pointer(constants::FREQ_LOWER_EDGE));
}


void Annotation::set_freq_lower_edge(double freq_lower_edge)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_freq_lower_edge(freq_lower_edge));

    this->set(sigmf_io::pointer(constants::FREQ_LOWER_EDGE), freq_lower_edge);
}


std::optional<double> Annotation::freq_upper_edge() const
{
    return this->get_optional<double>(sigmf_io::pointer(constants::FREQ_UPPER_EDGE));
}


void Annotation::set_freq_upper_edge(double freq_upper_edge)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_freq_upper_edge(freq_upper_edge));

    this->set(sigmf_io::pointer(constants::FREQ_UPPER_EDGE), freq_upper_edge);
}


std::optional<std::string> Annotation::label() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::LABEL));
}


void Annotation::set_label(const std::string& label)
{
    this->set(sigmf_io::pointer(constants::LABEL), label);
}


std::optional<std::string> Annotation::comment() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::COMMENT));
}


void Annotation::set_comment(const std::string& comment)
{
    this->set(sigmf_io::pointer(constants::COMMENT), comment);
}


std::optional<std::string> Annotation::generator() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::GENERATOR));
}


void Annotation::set_generator(const std::string& generator)
{
    this->set(sigmf_io::pointer(constants::GENERATOR), generator);
}


std::optional<std::string> Annotation::uuid() const
{
    return this->get_optional<std::string>(sigmf_io::pointer(constants::UUID));
}


void Annotation::set_uuid(const std::string& uuid)
{
    if(this->is_validation_strict())
        SpecValidatorBase::raise_error(this->validation_context_.validator->check_uuid(uuid));

    this->set(sigmf_io::pointer(constants::UUID), uuid);
}

} // end sigmf_io namespace
