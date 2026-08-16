#ifndef SIGMF_ANNOTATION_H
#define SIGMF_ANNOTATION_H

#include "json_base.h"
#include "SigMFUUID.h"
#include <jsoncons/json.hpp>
#include <optional>
#include <cstdint>
#include <string>

class SigMFAnnotation : public JSONBase
{
public:
    explicit SigMFAnnotation(const jsoncons::json& data = jsoncons::json());

    // returns a jsoncons::json initialized with default SigMF values.
    static jsoncons::json default_data();

    int64_t sample_start() const;
    void set_sample_start(int64_t sample_start);
    std::optional<int64_t> sample_count() const;
    void set_sample_count(int64_t sample_count);
    std::optional<double> freq_lower_edge() const;
    void set_freq_lower_edge(double freq_lower_edge);
    std::optional<double> freq_upper_edge() const;
    void set_freq_upper_edge(double freq_upper_edge);
    std::optional<std::string> label() const;
    void set_label(const std::string& label);
    std::optional<std::string> comment() const;
    void set_comment(const std::string& comment);
    std::optional<std::string> generator() const;
    void set_generator(const std::string& generator);
    std::optional<SigMFUUID> uuid() const;
    void set_uuid(const std::string& uuid);
};

#endif // SIGMF_ANNOTATION_H

