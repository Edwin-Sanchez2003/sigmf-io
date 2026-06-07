#include <iostream>

#include "SigMFDataType.h"
#include "SigMFDataset.h"
#include "sigmf.h"

using SigMFCapture = sigmf::VariadicDataClass<sigmf::core::CaptureT>;

int main()
{
    SigMFDataset sigmfData = SigMFDataset("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-data", SigMFDataType("cf32_le"), 1, 0);

    std::cout << sigmfData.getDataType().getRawDataType() << '\n';
    std::vector<SigMFCapture> captures;
    captures.clear();
    for(const std::complex<double>& sample: sigmfData.getSamples(captures, 0, 100))
        std::cout << sample.real() << " + " << sample.imag() << "i\n";
    return 0;
}
