#include <catch2/catch_test_macros.hpp>

#include <ranges>
#include <iostream>
#include <string>

#include "sigmf_io/recording.h"

TEST_CASE("Initialize Recording", "[recording]")
{
    // construct a Recording from a string path.
    const sigmf_io::Recording rec("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-meta");
    SUCCEED("Constructed without throwing");
}


TEST_CASE("Get Recording Paths", "[recording]")
{
    // construct a Recording from a string path.
    const sigmf_io::Recording rec("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-meta");

    std::cout << rec.data_path() << '\n';
    std::cout << rec.meta_path() << '\n';

    // print the data & meta paths.
    SUCCEED("Constructed and printed without throwing.");
}


TEST_CASE("Get Recording Capture Samples - double", "[recording]")
{
    // construct a Recording from a string path.
    const sigmf_io::Recording rec("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-meta");

    std::vector<std::complex<double>> samples = rec.get_capture_samples<std::complex<double>>(0, 1);

    // print first 10 samples of the capture.
    for(const std::complex<double>& sample: samples | std::views::take(10))
        std::cout << sample.real() << " + " << sample.imag() << "i\n";
    std::cout << '\n';

    // print the data & meta paths.
    SUCCEED("Constructed and printed without throwing.");
}


TEST_CASE("Get Recording Capture Samples - float", "[recording]")
{
    // construct a Recording from a string path.
    const sigmf_io::Recording rec("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-meta");

    std::vector<std::complex<float>> samples = rec.get_capture_samples<std::complex<float>>(0, 1);

    // print first 10 samples of the capture.
    for(const std::complex<float>& sample: samples | std::views::take(10))
        std::cout << sample.real() << " + " << sample.imag() << "i\n";
    std::cout << '\n';

    // print the data & meta paths.
    SUCCEED("Constructed and printed without throwing.");
}


TEST_CASE("Get Recording Capture Samples - int32", "[recording]")
{
    // construct a Recording from a string path.
    const sigmf_io::Recording rec("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-meta");

    // note: This doesn't make much sense if data on disk is normalized -> silent lossy conversion...
    std::vector<std::complex<int32_t>> samples = rec.get_capture_samples<std::complex<int32_t>>(0, 1);

    // print first 10 samples of the capture.
    for(const std::complex<int32_t>& sample: samples | std::views::take(10))
        std::cout << sample.real() << " + " << sample.imag() << "i\n";
    std::cout << '\n';

    // print the data & meta paths.
    SUCCEED("Constructed and printed without throwing.");
}


TEST_CASE("Get Recording Annotation Samples", "[recording]")
{
    // construct a Recording from a string path.
    const sigmf_io::Recording rec("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-meta");

    std::vector<std::complex<double>> samples = rec.get_samples<std::complex<double>>(rec.meta.annotations()[0], 1);

    // should retrieve 978944 samples
    REQUIRE(samples.size() == 978944);

    // print first 10 samples of the annotation.
    for(const std::complex<double>& sample: samples | std::views::take(10))
        std::cout << sample.real() << " + " << sample.imag() << "i\n";
    std::cout << '\n';

    // print the data & meta paths.
    SUCCEED("Constructed and printed without throwing.");
}
