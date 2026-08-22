# BurstRFBackend
This is a C++ repository designed to interface with SigMFDatasets, SigMFMetdata, SigMFRecordings, SigMFCollections, and SigMFArchives, with a focus on correctness.

## Initial Interface Design

* fix header guards to work more generally when incorporated in other projects.
* fix member vars to be underscores for member variables, rather than camelCase.
* fix member vars to have underscore after name, as a convention.
* fix Datatype enum naming to be lowercase for *Type -> *type: (SampleType -> Sampletype)?
* fix include order to be consistent (stdlib -> dependencies -> local lib? look up the standard convention).
* general header & impl convention cleanup!!!
* test code folder -> move 'main.cpp' to a 'test.cpp' folder. Setup CMakeLists.txt to allow exclusion of building test cases for users of the library.

* inherit for ~global~, ~capture~, ~annotation~, and geolocation.
* metadata class composes these objects.
* how much of jsoncons::json to expose???
    * how much of the jsoncons::json interface should I make the objects compatible with?

### Phase 1: Initial Interface Structure
* Getters/setters ought to validate input & throw an error if it's invalid (sample_start, sample_count, etc.)
* SigMFRecording to start out.
* Constructor initializes the jsoncons object.
    - Don't worry about general schema enforcement yet (outside individual value policy enforcement) - later addition.
* Implement core interface:
    - Global Object
    - Captures array/vector object.
    - Annotations array/vector object.
    - each of these have a getter/setter for the core namespace values. (again, don't worry about value enforcement yet).
* SigMFDataset object initialization & interface (read samples from disk).
* Note: read header bytes out to use as bytes if they exist!
* update hard-coded strings to const string values in namespaces (sample_rate should be sigmf_io::core::global::SAMPLE_RATE (a std::string)).

## Phase 2: Schema Validation
* Add in schema validation - eager/lazy/none for the core namespace.
* Add in schema validation for extensions.
* Make sure it still works with/handles/ignores keys outside the core namespace...

## Phase 3: Extensions & General key/value insertion.
* Allow users to add their own extensions & keys/values/object structures.

## Phase 4: Read/Write Interface
* Add in support to read & write datasets, with proper warnings for overwriting datasets.

## Phase 5: Comprehensive Schema Enforcement
* There are schema-related requirements that aren't obvious/lack implicit enforcement - go through and make sure that they are enforced properly.
    - Capture & Annotation array/sample order - ordered by samples.
        - Maybe then order by some other field for consistency? I dunno...
* Begin writing test cases!!!
        
## Phase 6: Complete Error Handling & Clear Error Messages
* Error messages are well-designed around Eager/Lazy/None schema enforcement.
* Error messages describe the problem completely.

## Phase 7: Annotation / Capture / Meta / Data / Recording / Collection / Archive
* Make sure interface is properly broken into parts based on structures from the spec, and are capable of interfacing with each other naturally.

## Notes / ToDo
* Because the json objects are separate (global, capture, annotation, etc.) Things get a little wonky. Does a user use normal json pointers, or when interacting with each object do they have to write pointers that work in that context (not /global/core:datatype when using a global object, but /core:datatype).
* Currently, going to throw errors when we come across malformed datasets. Later, we will want to have options for lenient/strict loading to handle cases where a user failed to create a valid SigMF dataset -> do we want to error on-load? when it hits the function value? on-write? etc.
* Geolocation currently doesn't support everything found in the spec, and doesn't support the version differences between what can be found in the global vs. the capture fields.
* Move headrs/impl files into sigmf_io folders.
* when moving into namespace & renaming files, make sure to update the header guards to use #ifndef SIGMF_IO_GLOBAL_H, etc. So that we avoid silent errors in case other use similarly named files.
* There's a difference between "default" on load and "default" on write.
    - default on load: default value is assumed when the value is missing in a recording.
    - default on write: default value is assumed when a user does not specify a value to be written, and is substituted in during write-time.
* For non-conforming datasets, allow the user to define & extract header_bytes & trailing_bytes fields!
    - MVP-extract raw bytes & let the user mess with it (convert to data types, etc.).
* implement SigMF metadata loading, using json library. Interfaces should:
    - support SigMF Core Namespace.
    - support adding/setting/getting fields that are NOT defined by the core namespace.
    - support extension objects that are user-defined.
* May need to implement/expose JSONPath and JMESPath along with JSONPointer for user convenience...
    - maybe need to expose JSON object itself???
* For error handling, may want to make base class for custom types to implement validation functionality (virtual function they all must implement).
* SigMF Extensions should be user-extendable base classes - a user can implement a custom C++ class that defines an extension interface, for convenience.
* Table of schemas corresponding to versions of the SigMF specification format.
* Ability for user-based extension JSON schemas to be added to validate portions of the JSON data.
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
* Should consider in a later version, a facade/wrapper around jsoncons to abstract the json backend out - this way the user can decide to swap out the json backend OR I can swap it out if I think it matters later... for now, jsoncons is exposed in metadata constructors & when adding arbitrary fields.
