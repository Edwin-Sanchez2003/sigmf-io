# BurstRFBackend
This is a C++ repository designed to interface with SigMFDatasets, SigMFMetdata, SigMFRecordings, SigMFCollections, and SigMFArchives, with a focus on correctness.

## NotesTODO:
 * ~Implement int64_t size() on SigMFDataset class.~
 * SigMFDataset class should handle trailing_bytes.
 * SigMFDataset class should handle header_bytes (NOTE: need the concept of a 'capture' for this to work properly.)  -> This means updating loadSamples() to use captures, if given, to offset samples from their sample bytes.
 * Update interface to use captures instead of sampleStart & sampleCount. Since each capture can have header_bytes, you have to account for those when you load IQ data from the file - this means a sampleStart & sampleCount range needs to understand what captures it spans across to identify the header_bytes.
 * Support Dataset .size() function -> a function of header_bytes, footer_bytes, and file size. Should also have a convenience function to support channelSize(int64_t) to get the size of a certain channel (handle edge case of incomplete channel streams - one channel as one more sample than the others, etc).
 * Implement SigMFRecording class.
 * offset is NOT needed by the SigMFDataset class -> SigMFRecording class should manage offset. In this case, 'offset'means the global indexing of the annotations. SigMFRecording will interact with annotations, so it will handle offsets.
 * Setup SigMFRecording class to be the main public interface of which to interact with a SigMF Dataset & Metadata.
 *  -> A SigMFDataset doesn't know of it's own header bytes; you need metadata to do that (in fact, you need the meta data in order to interact with most of a SigMFDataset.
 * implement SigMFCollections.
 * validate C++ interface works for all valid SigMF Datasets, Collection, and even Non-Conforming Datasets.
 * clean up public interface & setup test cases?
 * Something to figure out: Should this interface be for LOADING SigMFRecordings, or BUILDING SigMF Recordings? Or Both?
 * If Both, we need to reconcile the problem of RAII - if things have to be "complete" to be initialized, then this may place a significant burden on the user when building a SigMF dataset - they will need to make sure they provide all correct & complete data in order to support the SigMF format. Maybe not that bad, we will see...
 * Consider error handling after complete - need robust error messages to make SigMF dataset interactions easy/simple.
 * Consider speed after complete - may require re-writing SigMFDataset interface to be template class in general to support the DataType (templatizing getSamples with DataTypes).This way we can have a new function to load data for every supported
 * DataType, which would be optimal speed for data loading, rather than only having an interface for std::complex<double>.
 * Along with this, we should offer a conversion to any supported data type for user's convenience so they can quickly convert between data types as needed, or to pick one data type to do everything in (when the user is not worried about time/space efficiency).
 * Maybe once complete, provide a Python Wrapper? And a path to provide wrappers for other languages?
 * Maybe make it a header-only library? one include?
 * Question: Should this be thread-safe??? If we're dealing with threading, and people can build datasets as well as write to them, we could get race conditions for checking for computed values. Overall, could be messy.
 * Hard question: Should SigMFDataset objects be self-encapsulating? for instance, trailing_bytes is needed for a lot of calculations, so it would make sense to store it as a member variable. However, that value can change if you're building a SigMFdataset object, and so now you're managing it at the record-level (metadata-level) and in the Dataset object. This sucks. So maybe it should become a function input wherever it's needed when working with a Dataset??? Maybe you should have a pointer to a Recording object? Maybe two constructors - one where we build a "fake" recording object & give it the bare-minimum dataset metadata, and one where we pass in a real sigmf metadata reference pointer... This would keep the interface simple inside, but allow changes that reflect across objects that are related??? I don't know...
 * Question: Should we bail on libsigmf? Honestly a headache to maintain coupling. No clear data type to interact with - how do I know what I'm working with??? Tradeoff is it could get messy to read json data into C++, and then I have to build classes for the respective data types... may still be better than figuring out wtf this is: using SigMFCapture = sigmf::VariadicDataClass<sigmf::core::CaptureT>;
 * I honestly like the std::optional<> usage though, as it forces you to handle when values are missing. This is good practice, but the template hell is hard to follow...
 * Consider eventually moving to Eigen for efficient vectors? Or maybe some other one... to avoid complexity of implementing to handle complex vs. real / primitive data type / endianness.
