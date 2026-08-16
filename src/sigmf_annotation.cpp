#include "sigmf_annotation.h"
#include "json_base.h"

SigMFAnnotation::SigMFAnnotation(const jsoncons::json& data)
    : JSONBase(SigMFAnnotation::default_data(), data)
{}


jsoncons::json SigMFAnnotation::default_data()
{
    jsoncons::json defaults;
    defaults["core:sample_start"] = 0;
    return defaults;
}


int64_t SigMFAnnotation::sample_start() const
{
    return this->get<int64_t>("/core:sample_start");
}


void SigMFAnnotation::set_sample_start(int64_t sample_start)
{
    this->set("/core:sample_start", sample_start);
}


std::optional<int64_t> SigMFAnnotation::sample_count() const
{
    return this->get_optional<int64_t>("/core:sample_count");
}


void SigMFAnnotation::set_sample_count(int64_t sample_count)
{
    this->set("/core:sample_count", sample_count);
}


std::optional<double> SigMFAnnotation::freq_lower_edge() const
{
    return this->get_optional<double>("/core:freq_lower_edge");
}


void SigMFAnnotation::set_freq_lower_edge(double freq_lower_edge)
{
    this->set("/core:freq_lower_edge", freq_lower_edge);
}


std::optional<double> SigMFAnnotation::freq_upper_edge() const
{
    return this->get_optional<double>("/core:freq_upper_edge");
}


void SigMFAnnotation::set_freq_upper_edge(double freq_upper_edge)
{
    this->set("/core:freq_upper_edge", freq_upper_edge);
}


std::optional<std::string> SigMFAnnotation::label() const
{
    return this->get_optional<std::string>("/core:label");
}


void SigMFAnnotation::set_label(const std::string& label)
{
    this->set("/core:label", label);
}


std::optional<std::string> SigMFAnnotation::comment() const
{
    return this->get_optional<std::string>("/core:comment");
}


void SigMFAnnotation::set_comment(const std::string& comment)
{
    this->set("/core:comment", comment);
}


std::optional<std::string> SigMFAnnotation::generator() const
{
    return this->get_optional<std::string>("/core:generator");
}


void SigMFAnnotation::set_generator(const std::string& generator)
{
    this->set("/core:generator", generator);
}


std::optional<SigMFUUID> SigMFAnnotation::uuid() const
{
    std::optional<std::string> uuid = this->get_optional<std::string>("/core:uuid");
    if(uuid.has_value()) {
        return SigMFUUID(uuid.value());
    }
    return std::nullopt;
}


void SigMFAnnotation::set_uuid(const std::string& uuid)
{
    this->set("/core:uuid", uuid);
}
