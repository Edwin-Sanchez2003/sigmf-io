# BurstRFBackend
This is a C++ repository designed to interface with SigMFDatasets, SigMFMetdata, SigMFRecordings, SigMFCollections, and SigMFArchives, with a focus on correctness.

## Notes / ToDo
* Implement minor classes that enforce schema logic - doi, license, sha512, etc...
* For error handling, may want to make base class for custom types to implement validation functionality (virtual function they all must implement).
* SigMF Extensions should be user-extendable base classes - a user can implement a custom C++ class that defines an extension interface, for convenience.
* Seem to use regex alot - may want to make one place with all of the patterns?
* Implement supported versions as a const map where a specific version is mapped to a certain implementation of the SigMFSpecEnforcementVisitor - when a new spec drops, we simply inherit from the last implementation & re-implement functions that change. That would be bitchin'.
* Convert SigMF namespace types to classes & enforce at construction.
    - They will need to be used for both reading & writing - don't worry about writing for now, future me's problem...
    - Follow spec logic to fill in fields at construction time.
* Create sigmf_io namespace & put all classes in there...
    - remove SigMF prepend to each class...
* Schema enforcement should be have 2 dimensions:
    1. **Timing** - *When* the schema is enforced. This can be:
        - **Eager** - at value-setting time. This will be the most helpful, as things throw an error right where they are set.
        - **Lazy** - at write-time. This is convenient when you need to set values temporarily that may be incorrect until you have good placeholders. 
        - **None** - never. You don't care about it being correct (at your own peril).
-    2. **Enforcement** - *What* gets enforced. This should be based on the  MUST, SHALL, SHOULD, etc. phrasing, to give good enough variety of enforcement for users.
    * The defaults should be EAGER & MUST (Strictest).
* Constructors should account for the bounds that are allowed, given the specification, and should also point to which part of the specification indicates why the data fails if an error has to be thrown.
    - There should be error-handling used to catch when the user doesn't care if values aren't bounded properly - instead of erroring out, the program should continue running until it crashes, rather than early stopping. This will help when the user doesn't care about certain metadata (even if it's malformed), and still wants to run the program.
* SigMFRecording Capture Interface should be easy to use:
    - Get captures' samples with a simple function call.
    - Get annotations strictly within a capture.
    - Get annotations partially overlapping with a capture.
    - Annotations should be able to identify which captures it overlaps with.
    - Annotations should know which capture it starts in.
    - Annotations should know which capture it ends in.
* What containers should be returned to the user for RF data? Vector? Something else? What gives the most flexibility? Most convenience?
    - Leaning towards most flexible container by default.
    - Extensions for certain well-known libraries (Eigen, etc.)
* Implement bracket indexing for SigMFDatasets - can treat it like a std::vector or something similar (std::iterable?). Implement that interface on it to make it more compatible with other tools??? May require re-defining the interface...
* Current rule: Any data type that requires parsing gets a custom data type. Anything that has a simple restriction on the domain uses the raw data type, and is validated using comparison functions.
* Does memory mapping do anything right now? - we create a vector in-memory and then load samples when the vector is initialized (doesn't this defeat the purpose)? need to double-check to see if this is even worth the effort/dependency...
    - If we end up implementing SigMFDataset with the iterable/vector interface, then we could make it work as a proxy to the memmapped dataset - only loading data into memory when actually indexed by the user. I think vectors can work like this already, so may just be a matter of pointing the vector to the correct spots on disk/intializing and returning one? I don't know... C++ ranges, etc.? Look into the proxy vector thing...
* Need to enforce data types and boundaries for metadata fields, when included.
    - Allow for user to enforce the format strictly.
    - Allow the user to disable strict enforcement (runtime-errors will then be used if a param is attempted to be read but doesn't exist).
* Implement SigMFRecording class.
* Since offset is a SHOULD and not a SHALL, we need to account for when core:sample_start values are NOT offset by this value - we would need to ignore offset during data loading. This is a potential bug for people who make SigMFMetadata, define an offset value, BUT don't actually offset their capture and annotation core:sample_start values by this offset - this code would not load it properly unless they manually fix offset to be zero in the SigMFDataset. I'll have to think about how to handle this...
* Think of pattern to support MUST, MUST NOT, REQUIRED, SHALL, SHALL_NOT, SHOULD, SHOULD_NOT, RECOMMENDED, MAY, OPTIONAL as they come up for certain fields.
* Setup SigMFRecording class to be the main public interface of which to interact with a SigMF Dataset & Metadata.
* allow for Meta-Only data reading.
* implement SigMFCollections.
* validate C++ interface works for all valid SigMF Datasets, Collections, Archives, and Non-Conforming Datasets.
* clean up public interface & setup test cases?
* Something to figure out: Should this interface be for LOADING SigMFRecordings, or BUILDING SigMF Recordings? Or Both?
* If Both, we need to reconcile the problem of RAII - if things have to be "complete" to be initialized, then this may place a significant burden on the user when building a SigMF dataset - they will need to make sure they provide all correct & complete data in order to support the SigMF format. Maybe not that bad, we will see...
* Consider error handling after complete - need robust error messages to make SigMF dataset interactions easy/simple.
* Consider speed after complete - may require re-writing SigMFDataset interface to be template class in general to support the DataType (templatizing getSamples with DataTypes).This way we can have a new function to load data for every supported DataType, which would be optimal speed for data loading, rather than only having an interface for std::complex\<double>.
    - At this point, this means we need to write fast-path function overloads for specific cases - Conforming SigMFDatasets, one-channel datasets, the user loading the same type as what's on disk, etc.
* Maybe once complete, provide a Python Wrapper? And a path to provide wrappers for other languages?
* Maybe make it a header-only library? one include?
* Question: Should this be thread-safe??? If we're dealing with threading, and people can build datasets as well as write to them, we could get race conditions for checking for computed values. Overall, could be messy.
* Hard question: Should SigMFDataset objects be self-encapsulating? for instance, trailing_bytes is needed for a lot of calculations, so it would make sense to store it as a member variable. However, that value can change if you're building a SigMFdataset object, and so now you're managing it at the record-level (metadata-level) and in the Dataset object. This sucks. So maybe it should become a function input wherever it's needed when working with a Dataset??? Maybe you should have a pointer to a Recording object? Maybe two constructors - one where we build a "fake" recording object & give it the bare-minimum dataset metadata, and one where we pass in a real sigmf metadata reference pointer... This would keep the interface simple inside, but allow changes that reflect across objects that are related??? I don't know...
* Question: Should we bail on libsigmf? Honestly a headache to maintain coupling. No clear data type to interact with - how do I know what I'm working with??? Tradeoff is it could get messy to read json data into C++, and then I have to build classes for the respective data types... may still be better than figuring out wtf this is: using SigMFCapture = sigmf::VariadicDataClass\<sigmf::core::CaptureT>;
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
