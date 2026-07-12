# BurstRFBackend
This is a C++ repository designed to interface with SigMFDatasets, SigMFMetdata, SigMFRecordings, SigMFCollections, and SigMFArchives, with a focus on correctness.

## Notes / ToDo
 * ~Implement int64_t size() on SigMFDataset class.~
 * ~SigMFDataset class should handle trailing_bytes.~ -> Since SigMFDataset.size() already factors in trailing_bytes, and this is the value used to load samples, then this is OK.
 * ~SigMFDataset class should handle header_bytes (NOTE: need the concept of a 'capture' for this to work properly.)  -> This means updating loadSamples() to use captures, if given, to offset samples from their sample bytes.~
 * ~Re-write of the loadSamples algorithm - give 2 template data types. The first type will be the type you want to load, the second will be the type the data is stored in on disk.~
    - NOTE: The reason you hide loadSamples in getSamples is so the SigMFDataset class can hide the process of identifying the data type, using the data type object. This way, the user only specifies the template type of the data they want loaded in memory as a template parameter on the getSamples function call, and doesn't have to think about what data is on disk.
 * Need to make interfaces for fields loaded from metadata. Constructors should account for the bounds that are allowed, given the specification, and should also point to which part of the specification indicates why the data fails if an error has to be thrown.
    - There should be error-handling used to catch when the user doesn't care if values aren't bounded properly - instead of erroring out, the program should continue running until it crashes, rather than early stopping. This will help when the user doesn't care about certain metadata (even if it's malformed), and still wants to run the program.
 * ~SigMFDataset class loadSamples should also return any supported data type as the vector. This means the user should be able to specify what data type is on disk (using a SigMFDataType) and then also specify what data type to load into the vector (so the template function should have two template parameters - one for the data on disk, one for the type the user wants.~
    - ~TODO: write README.md file on SigMFDataset Spec rules - and what needs to be thought about when loading data from disk...~ (header_bytes, capture/annotation order, etc.)...
 * ~Make loadSamples the public interface to getting data out of a SigMFDataset object?~ -> NO - needed for user input validation & mapping known on-disk SigMF data types to template function calls. This may change later, but it has a purpose for now.
 * Implement bracket indexing for SigMFDatasets - can treat it like a std::vector or something similar (std::iterable?). Implement that interface on it to make it more compatible with other tools??? May require re-defining the interface...
 * Does memory mapping do anything right now? - we create a vector in-memory and then load samples when the vector is initialized (doesn't this defeat the purpose)? need to double-check to see if this is even worth the effort/dependency...
    - If we end up implementing SigMFDataset with the iterable/vector interface, then we could make it work as a proxy to the memmapped dataset - only loading data into memory when actually indexed by the user. I think vectors can work like this already, so may just be a matter of pointing the vector to the correct spots on disk/intializing and returning one? I don't know... C++ ranges, etc.? Look into the proxy vector thing...
 * ~Update interface to use captures as an optional interface along with sampleStart & sampleCount (ie. given a set of captures, pull all of the samples that belong to them). Since each capture can have header_bytes, you have to account for those when you load IQ data from the file - this means a sampleStart & sampleCount range needs to understand what captures it spans across to identify the header_bytes. This can be implemented on the SigMFRecording class as a convenience function.~ -> Implemented into SigMFDataset since this class is designed to handle all possible SigMFDataset loading.
    - Need to define which convenience functions should exist in the SigMFDataset class and which should exist on the SigMFRecording class...
 * ~Support Dataset .size() function -> a function of header_bytes, footer_bytes, and file size. Should also have a convenience function to support channelSize(int64_t) to get the size of a certain channel (handle edge case of incomplete channel streams - one channel as one more sample than the others, etc).~
 * Need to enforce data types and boundaries for metadata fields, when included.
    - Allow for user to enforce the format strictly.
    - Allow the user to disable strict enforcement (runtime-errors will then be used if a param is attempted to be read but doesn't exist).
    
 * Implement SigMFRecording class.
 * ~offset is NOT needed by the SigMFDataset class -> SigMFRecording class should manage offset. In this case, 'offset' means the global indexing of the annotations. SigMFRecording will interact with annotations, so it will handle offsets.~ -> Incorrect. The spec states that offsets influence the values of core:sample_start within captures and annotations - specifically, core:sample_start values SHOULD be greater than or equal to this value. This means we must account for it in data loading...
* Since offset is a SHOULD and not a SHALL, we need to account for when core:sample_start values are NOT offset by this value - we would need to ignore offset during data loading. This is a potential bug for people who make SigMFMetadata, define an offset value, BUT don't actually offset their capture and annotation core:sample_start values by this offset - this code would not load it properly unless they manually fix offset to be zero in the SigMFDataset. I'll have to think about how to handle this...
 * Setup SigMFRecording class to be the main public interface of which to interact with a SigMF Dataset & Metadata.
 *  -> A SigMFDataset doesn't know of it's own header bytes; you need metadata to do that (in fact, you need the meta data in order to interact with most of a SigMFDataset.
 * allow for Meta-Only data reading.
 * implement SigMFCollections.
 * validate C++ interface works for all valid SigMF Datasets, Collections, Archives, and Non-Conforming Datasets.
 * clean up public interface & setup test cases?
 * Something to figure out: Should this interface be for LOADING SigMFRecordings, or BUILDING SigMF Recordings? Or Both?
 * If Both, we need to reconcile the problem of RAII - if things have to be "complete" to be initialized, then this may place a significant burden on the user when building a SigMF dataset - they will need to make sure they provide all correct & complete data in order to support the SigMF format. Maybe not that bad, we will see...
 * Consider error handling after complete - need robust error messages to make SigMF dataset interactions easy/simple.
 * Consider speed after complete - may require re-writing SigMFDataset interface to be template class in general to support the DataType (templatizing getSamples with DataTypes).This way we can have a new function to load data for every supported DataType, which would be optimal speed for data loading, rather than only having an interface for std::complex<double>.
    - At this point, this means we need to write fast-path function overloads for specific cases - Conforming SigMFDatasets, one-channel datasets, the user loading the same type as what's on disk, etc.
* ~Along with this, we should offer a conversion to any supported data type for user's convenience so they can quickly convert between data types as needed, or to pick one data type to do everything in (when the user is not worried about time/space efficiency).~
 * Maybe once complete, provide a Python Wrapper? And a path to provide wrappers for other languages?
 * Maybe make it a header-only library? one include?
 * Question: Should this be thread-safe??? If we're dealing with threading, and people can build datasets as well as write to them, we could get race conditions for checking for computed values. Overall, could be messy.
 * Hard question: Should SigMFDataset objects be self-encapsulating? for instance, trailing_bytes is needed for a lot of calculations, so it would make sense to store it as a member variable. However, that value can change if you're building a SigMFdataset object, and so now you're managing it at the record-level (metadata-level) and in the Dataset object. This sucks. So maybe it should become a function input wherever it's needed when working with a Dataset??? Maybe you should have a pointer to a Recording object? Maybe two constructors - one where we build a "fake" recording object & give it the bare-minimum dataset metadata, and one where we pass in a real sigmf metadata reference pointer... This would keep the interface simple inside, but allow changes that reflect across objects that are related??? I don't know...
 * Question: Should we bail on libsigmf? Honestly a headache to maintain coupling. No clear data type to interact with - how do I know what I'm working with??? Tradeoff is it could get messy to read json data into C++, and then I have to build classes for the respective data types... may still be better than figuring out wtf this is: using SigMFCapture = sigmf::VariadicDataClass<sigmf::core::CaptureT>;
 * I honestly like the std::optional<> usage though, as it forces you to handle when values are missing. This is good practice, but the template hell is hard to follow...
 * Consider eventually moving to Eigen for efficient vectors? Or maybe some other one... to avoid complexity of implementing to handle complex vs. real / primitive data type / endianness.
 * One option is to write optional header files that implement interfaces for certain common C++ libraries - Eigen, etc. - so that users of the library can optionally compile those interfaces for simpler usage, rather than converting to custom container formats for samples.
    - Also include common container interface operations (C++ ranges, .begin(), .end() interfaces, etc.) so that users can easily interface with a dataset themselves.
 * API to allow a user to treat a NCD as a runtime SigMF dataset. You don't have to necessarily generate .sigmf-meta files, you can use the interface to describe only the needed information, then let the program run in memory.
 * Extension Builder - make a tool to help add SigMF specification extensions for the project - a GUI interface lets you build the structure & requirements (data types, required vs. optional fields). This will enable groups to do the specifics of their work easily.
    * Also allow users to ingest an existing file & build a specification from it. This means reading a file, infering the data types, and suggesting the rules automatically. This will help with custom-made specs that were written first and defined rigidly later.
    * May also need to handle newly defined data types... YIKES.
* Update the interfaces to each class to implement different contruction options (copy, move, const, static, etc.) so that the user can make the most of each function.
* Maybe make SigMFDataType enums castable to strings, for error handling clarity...
* Make sure spec is validated - captures & annotations should be sorted before used. Captures should never overlap!
* Need to remember to handle versioning... both of this repo AND of SigMF versions.
* Need to explicitly say if the data returned when loaded is normalized / allow user to specify if they want the loaded data to be normalized \[-1, 1\].?

## SigMFDataset Algorithm
1. Validate User Input
    1. ~Check sampleStart & sampleCount are in-bounds.~
    2. ~Check channel is in-bounds.~
    * This uses SigMFDataset.size(channel) function.
2. ~Get start byte of file as byte.~
3. ~Allocate vector of memory of size sampleCount.~
4. ~If NO capture data (ie. no header_bytes), apply formula to get to sample byte index: (NOTE: offset will be handled with respect to metadata).~
`index_offset_samples = (sample_start - offset) * num_channels + (channel - 1)` 
`index_offset_bytes = index_offset_samples * bytes_per_sample`
5. ~If capture data (ie. header_bytes) exists, then data is offset by header_bytes as well:~
`index_offset_bytes += accumulated_header_bytes`
6. ~Loop to aggregate samples from disk.~
    1. ~Cast current sentry (initially index_offset_bytes) to on-disk primitive type.~
    2. ~Perform endianness swap.~
    3. ~Push back into vector. If complex, read both components (real & complex).~
    4. ~Increment by formula:~ `bytes_per_sample * num_channels`
    5. ~If we pass a capture boundary, offset by header_bytes again.~
7. ~Cast to user-specified data type.~
