#include <iostream>

#include "SigMFDataType.h"
#include "SigMFDataset.h"
#include "sigmf.h"

using SigMFCapture = sigmf::Capture<sigmf::core::DescrT>;

int main()
{
    SigMFDataset sigmfData = SigMFDataset("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-data", SigMFDataType("cf32_le"), 1, 0);
    std::cout << sigmfData.getDataType().getRawDataType() << '\n';
    std::vector<SigMFCapture> captures;
    captures.clear();

    auto samples = sigmfData.getSamples<std::complex<double>>(captures, 0, 100);
    std::cout << "Sample count returned: " << samples.size() << '\n';

    for(const std::complex<double>& sample: samples)
        std::cout << sample.real() << " + " << sample.imag() << "i\n";

    return 0;
}


// Figuring out how I want the API to feel.
void test()
{

}
