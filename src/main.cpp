#include <iostream>

#include "SigMFRecording.h"
#include "SigMFDataType.h"
#include "SigMFDataset.h"
#include "sigmf.h"

using SigMFCapture = sigmf::Capture<sigmf::core::DescrT>;

int main()
{
    // Simple Initialization of SigMFDataset object.
    SigMFDataset sigmfData = SigMFDataset("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-data", SigMFDataType("cf32_le"), 1, 0);
    std::cout << sigmfData.getDataType().to_string() << '\n';
    std::vector<SigMFCapture> captures;
    captures.clear();

    auto samples = sigmfData.getSamples<std::complex<double>>(captures, 0, 100);
    std::cout << "Sample count returned: " << samples.size() << '\n';

    for(const std::complex<double>& sample: samples)
        std::cout << sample.real() << " + " << sample.imag() << "i\n";

    // Initial Initialization of SigMFRecording object.
    SigMFRecording recording = SigMFRecording("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-data");
    SigMFRecording();
    auto samples2 = recording.dataset.value().getSamples<std::complex<double>>(captures, 0, 10);
    std::cout << "Sample count returned: " << samples2.size() << '\n';

    for(const std::complex<double>& sample: samples2)
        std::cout << sample.real() << " + " << sample.imag() << "i\n";

    return 0;
}


// Figuring out how I want the API to feel.
void test()
{
    // Goal: Load SigMF Recording.
    //SigMFRecording recording = SigMFRecording("/path/to/file.sigmf-meta");

    // vector/some sort of iterable array-like structure of the first 100 samples (0-99) of the recording.
    // due to SigMF wonkiness, this is specifically the 1st channel.
    //recording[0:100]; // would need to implement array slicing as operator overloading...


    //recording.global


}
