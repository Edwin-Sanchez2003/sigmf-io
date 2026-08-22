#ifndef SIGMF_IO_ANNOTATION_H
#define SIGMF_IO_ANNOTATION_H

#include "sigmf_io/json_base.h"
#include "sigmf_io/uuid.h"
#include <jsoncons/json.hpp>
#include <optional>
#include <cstdint>
#include <string>

namespace sigmf_io {

class Annotation : public JSONBase
{
public:
    explicit Annotation(const jsoncons::json& data = jsoncons::json());

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
    std::optional<UUID> uuid() const;
    void set_uuid(const std::string& uuid);
};

} // end sigmf_io namespace

#endif // SIGMF_IO_ANNOTATION_H

