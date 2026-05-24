#include <iostream>

#include "SigMFDataType.h"
#include "SigMFDataset.h"

int main()
{
    /*
     * TODO:
     * * Update SigMFDataset class to handle channels.
     * * Implement SigMFRecording class.
     * * Setup SigMFRecording class to be the main public interface of which to interact with a SigMF Dataset & Metadata.
     *  -> A SigMFDataset doesn't know of it's own header bytes; you need metadata to do that (in fact, you need the meta
     *    data in order to interact with most of a SigMFDataset.
     * * implement SigMFCollections.
     * * validate C++ interface works for all valid SigMF Datasets, Collection, and even Non-Conforming Datasets.
     * * clean up public interface & setup test cases?
     */

    SigMFDataset sigmfData = SigMFDataset("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-data", SigMFDataType("cf32_le"), 0, 1, 0);

    std::cout << sigmfData.getDataType().getRawDataType() << '\n';
    for(const auto& sample: sigmfData.getSamples(0, 100))
        std::cout << sample.real() << " + " << sample.imag() << "i\n";
    return 0;
}
