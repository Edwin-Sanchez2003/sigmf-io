#ifndef SIGMFANNOTATION_H
#define SIGMFANNOTATION_H

#include <cstdint>
#include <optional>
#include<string>


struct SigMFAnnotation
{
    int64_t sample_start{0};
    std::optional<int64_t> sample_count;
    std::optional<double> freq_lower_edge;
    std::optional<double> freq_upper_edge;
    std::optional<std::string> label;
    std::optional<std::string> comment;
    std::optional<std::string> generator;
    std::optional<std::string> uuid;        // TODO: make custom class to enforce format.
};

#endif // SIGMFANNOTATION_H
