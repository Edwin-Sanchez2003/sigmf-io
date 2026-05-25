#include <iostream>

#include "SigMFDataType.h"
#include "SigMFDataset.h"

int main()
{
    /*
     * TODO:
     * * SigMFDataset class should handle footer_bytes.
     * * SigMFDataset class should handle header_bytes (NOTE: need the concept of a 'capture' for this to work properly.) Update interface
     *   to use captures instead of sampleStart & sampleCount. Since each capture can have header_bytes, you have to account for those when
     *   you load IQ data from the file - this means a sampleStart & sampleCount range needs to understand what captures it spans across to
     *   identify the header_bytes.
     * * Support Dataset .size() function -> a function of header_bytes, footer_bytes, and file size. Should also have a convenience
     *   function to support channelSize(int64_t) to get the size of a certain channel (handle edge case of incomplete channel streams -
     *   one channel as one more sample than the others, etc).
     * * Implement SigMFRecording class.
     * * offset is NOT needed by the SigMFDataset class -> SigMFRecording class should manage offset. In this case, 'offset'
     * means the global indexing of the annotations. SigMFRecording will interact with annotations, so it will handle offsets.
     * * Setup SigMFRecording class to be the main public interface of which to interact with a SigMF Dataset & Metadata.
     *  -> A SigMFDataset doesn't know of it's own header bytes; you need metadata to do that (in fact, you need the meta
     *    data in order to interact with most of a SigMFDataset.
     * * implement SigMFCollections.
     * * validate C++ interface works for all valid SigMF Datasets, Collection, and even Non-Conforming Datasets.
     * * clean up public interface & setup test cases?
     *
     * Something to figure out: Should this interface be for LOADING SigMFRecordings, or BUILDING SigMF Recordings? Or Both?
     * If Both, we need to reconcile the problem of RAII - if things have to be "complete" to be initialized, then this may place
     * a significant burden on the user when building a SigMF dataset - they will need to make sure they provide all correct &
     * complete data in order to support the SigMF format. Maybe not that bad, we will see...
     * * Consider error handling after complete - need robust error messages to make SigMF dataset interactions easy/simple.
     * * Consider speed after complete - may require re-writing SigMFDataset interface to be template class in general to support
     * the DataType (templatizing getSamples with DataTypes).This way we can have a new function to load data for every supported
     * DataType, which would be optimal speed for data loading, rather than only having an interface for std::complex<double>.
     * Along with this, we should offer a conversion to any supported data type for user's convenience so they can quickly convert
     * between data types as needed, or to pick one data type to do everything in (when the user is not worried about time/space
     * efficiency).
     * * Maybe once complete, provide a Python Wrapper? And a path to provide wrappers for other languages?
     * * Maybe make it a header-only library? one include?
     */

    SigMFDataset sigmfData = SigMFDataset("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-data", SigMFDataType("cf32_le"), 1, 0);

    std::cout << sigmfData.getDataType().getRawDataType() << '\n';
    for(const auto& sample: sigmfData.getSamples(0, 100))
        std::cout << sample.real() << " + " << sample.imag() << "i\n";
    return 0;
}
