# SigMF IO
`sigmf_io` is a C++ library designed to interface with SigMF data with the goal of standardizing implementation and usage of the SigMF Specification (https://sigmf.org/) across applications. This implementation places heavy emphasis on correctness and ease-of-use, with completeness and speed as secondary & tertiary goals.

## Purpose

The SigMF Specification was designed to facilitate recording and parsing of stored signal data (mostly Radio-Frequency data), with an emphasis on ease-of-use, accessibility, and flexibility. However, the specification has no complete and up-to-date implementation within the open-source community. Current users of the SigMF Specification must rely on a patchwork of out-of-date software resources that partially or inconsistently implement the specification. In most cases, users of the specification must devote a significant time investment in order to use the format as intended when writing code to store or parse data from scratch. Even then, a SigMF Specification user may mis-implement the specification in a way that makes it incompatible with other tools or external partners, requiring re-working collected metadata to be usable and reliable.

`sigmf_io` is meant to abstract the inner-workings and specification dependencies away from those who wish to use the SigMF specification, making it easier for users to focus on their intended application rather than spending time implementing to the SigMF specification. This library helps with:
* Maintaining SigMF files according to specific SigMF versions, helping to correctly enforce Specification requirements for both internal organizational preservation of data and to make data portable and shareable with external partners.
* Provides signal I/O interfaces to make it easy to read & write signal data to disk in the SigMF format.
* Includes core specification field object interfaces to pre-parse complex SigMF fields for you, and for easy tab completion when writing code, making it harder to make simple mistakes.

> This library is a third-party resource and has no affiliation with the maintainers of https://sigmf.org/.

## How to Include
*TODO*

## Example Usage
*TODO*

## Versioning
This repository relies on Semantic Versioning, in the form `MAJOR.MINOR.PATCH` -> ex: `0.1.0`:
* **MAJOR** - Incremented with changes that break the public API.
* **MINOR** - Incremented when new features are added that do not break the existing public API.
* **PATCH** - Incremented with bug fixes for algorithm correctness that are backward-compatible.

See https://semver.org/ for details.

> **Current Version:** `0.1.0`

## Dependencies
The current implementation of `sigmf_io` depends heavily on `jsoncons` (json library for dealing with metadata) and `mio` (memory-mapping library for reading data from disk). Users of this library will end up directly or indirectly interacting with these dependencies.
* jsoncons: https://github.com/danielaparker/jsoncons
* mio: https://github.com/vimpunk/mio

## Next Release
The core purpose of this library is to support correctness-by-default within the SigMF specification while giving users maximum flexibility when using the library (opting in or out of certain features as needed to support project-specific requirements). Upcoming releases focus heavily on:
* **Completeness** - supporting the entirety of the SigMF Specification, while maintaining correctness across existing implementation details. More concretely, this means supporting SigMF Collections & Archives, as well as providing better support for specification validation and interacting with SigMF fields that have specific properties (examples include `core:datatype`, which must match a specific  ABNF pattern and contains information about how to read SigMF signal data from disk, and `core:geolocation`, which itself is a json structure which is expected to match certain patterns for reliable reading & writing of data).
* **Efficiency** - identifying critical-path and computationally expensive operations to optimize for speed.

Additional future work includes packaging this libary for usage in other languages, such as `Python`, `Java`, and `Matlab`.

## Initial Interface Design
* Have per-value checks for set-time validation. Have jsoncons's json schema validation for document-wide checks. Also hand-roll functions to check across multiple fields.
    - subschema for global, annotation, captures, geolocation, etc?
* Some spec values won't need checking? (ex: description - who cares?).
    - We may want to leave in check functions for them anyway, in case we want to log/print suggestions (ie. core:label SHOULD be under X characters, etc.).
* Some spec values might be better off if they can be toggled on/off for validation (ie. STRICT by default, but disable checks for fields X, Y, and Z).
    - We can do this with overrides in the spec validator itself - user can initialize a spec validator, check/uncheck certain variables, then pass it to datasets to be used at the validator.
    - Also consider changing errors to warnings, etc.
* Current version of the spec validator simply checks unit-level values, and ignores anything not strictly required (MAY, SHOULD). Future versions should also check inter-field requirements (some fields' values and whether or not they should be used are dependent on other fields' values - ex: metadata_only MAY NOT be used with NCDs or the core:dataset field).
* JSONBase should be the base class for ~global~, ~capture~, ~annotation~, and geolocation.
* metadata class composes these objects.
* Initial version should work for Recordings -> basic read & write, test cases, then publish 0.1.0. Version 1.0 should have documentation.
* Pop off any forced conversion to custom data types -> the user should wrap the output of the metadata with this custom class explicitly rather than implicitly with the return value. This way, the user can always expect a string/object or json-supported output field when using the API, and then decide if they want to enforce spec-invariants on construction of custom objects.

* TODO: Drive sigmf version (core:version) from the specification version enforced, or force user to specify themselves (NOT hardcoded in default json in global.h).
* Future work may include auto-converters that can migrate datasets from older versions to newer versions. (Do this in a daisy-chain style, where to get from A.B.C -> X.Y.Z, you call any intermediate version migration functions, rather than trying to implement a one-to-many migration library).

* after version 1 -> geolocation?, extensions, collections, archives?
* Custom sigmf_io datatypes -> should be forced to use? no -> forced crashes, program may fail if incorrect format... not sure whether to go for convenience but risk crashing on malformed datasets or to go for robustness & add boilerplate when accessing fields... datatype is special because it is required for things to work at all. Version is also required... I think I need to think through handling these better in general -> should be consistent, and should give options. Pick one case to be the default, but make the other case optional (either default to forced usage of custom classes & optional unwrapped versions, or unwrapped by default & optional usage of custom classes).

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

We will wrestle with this for a while...
* how much of jsoncons::json to expose???
    * how much of the jsoncons::json interface should I make the objects compatible with?

## Notes / ToDo
* Because the json objects are separate (global, capture, annotation, etc.) Things get a little wonky. Does a user use normal json pointers, or when interacting with each object do they have to write pointers that work in that context (not /global/core:datatype when using a global object, but /core:datatype).
* the metadata API in general is atrocious - inconsistent usage of jsoncons::json interfaces, holes in enforcing the schema, missing convenience functions, awkward relationship between write-oriented & read-oriented metadata file usage.
* read vs. write interface is a real problem.
* metadata currently doesn't take in the spec validator as an argument & applies it at initialization & save unconditionally - this will be a problem later...
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
