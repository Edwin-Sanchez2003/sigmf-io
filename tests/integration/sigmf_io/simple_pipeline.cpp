#include <catch2/catch_test_macros.hpp>

#include <iostream>

#include "sigmf_io/recording.h"
#include "sigmf_io/capture.h"
#include "sigmf_io/datatype.h"
#include "sigmf_io/dataset.h"


TEST_CASE("Test Initialization of sigmf_io::Dataset", "[pipeline]")
{
    std::cout << "SIMPLE PIPELINE: Reading dataset and printing 100 samples...\n";
    // Simple Initialization of sigmf_io::Dataset object.
    sigmf_io::Dataset sigmf_data = sigmf_io::Dataset("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-data", sigmf_io::Datatype("cf32_le"), 1);
    std::cout << sigmf_data.datatype().to_string() << '\n';
    std::vector<sigmf_io::Capture> captures;
    captures.clear();

    auto samples = sigmf_data.get_samples<std::complex<double>>(captures, 0, 100);
    std::cout << "Sample count returned: " << samples.size() << '\n';

    for(const std::complex<double>& sample: samples)
        std::cout << sample.real() << " + " << sample.imag() << "i\n";
    std::cout << '\n';
}


// Figuring out how I want the API to feel.
TEST_CASE("Test Initialization of sigmf_io::Recording", "[pipeline]")
{
    std::cout << "SIMPLE PIPELINE: Read from Recording and print 10 samples...\n";
    // Initialization of sigmf_io::Recording object.
    sigmf_io::Recording recording = sigmf_io::Recording("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-meta");
    auto samples2 = recording.get_samples<std::complex<double>>(0, 10);
    std::cout << "Sample count returned: " << samples2.size() << '\n';

    for(const std::complex<double>& sample: samples2)
        std::cout << sample.real() << " + " << sample.imag() << "i\n";
    std::cout << '\n';
}
