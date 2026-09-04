# TODO List

## Pre 1.0.0 updates
* Settle into read-only for Datasets/raw signal data, with Metadata being editable. It is assumed the user will use built-in C++ support or their own task-specific interface to generate signal data & write to disk (or just basic C++ file I/O), with Metadata being written alongside it.
* ~update metadata interfaces to be lighter-weight facades of jsoncons::json objects, basically just extra methods on top of the object that are sigmf-specific. Make sure they are still composable.~
* refine separation between schema enforcement & metadata/recording classes.
* Refine dataset / samples interfaces for Dataset & Recording - right now, it uses std::vector\<T\>, which may not be the standard interface for what others want to use in their code -> is there a more general container, or can I define a container interface that a user can ingest into their preferred container type? Maybe include a basic vector interface for straight-forward usage, then provide a more generic interface for extensibility... Maybe this is a 1.1.0 update...
* Go ahead & implement is_ncd on top of Recording... & Metadata?

## Initial Interface Design
* **Read vs. Write API:** Current API does not strongly define read vs. write interfaces for the convenience classes. There is currently no support for writing baked into the API for raw signal data (only supports writing SigMF Metadata files).
* **Per-Value Set-Time Spec Validation:** Have per-value checks for set-time validation. Have jsoncons's json schema validation for document-wide checks. Also hand-roll functions to check across multiple fields.
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
* update hard-coded strings to const string values in namespaces (sample_rate should be sigmf_io::core::global::SAMPLE_RATE (a std::string)).

#### Some Things to Adjust
* I'm leaning towards making the global/capture/annotation interfaces thinner - possibly even inheriting from the jsoncons::json class & implementing on top of it. The main reason is that aggregating these types is a bit of a mess in the metadata class, and ends up taking away a lot of flexibility from the end user. A Metadata object isn't a json object, and you have to convert the vectors of captures & annotations to a jsoncons::json array before you can generate the actual jsoncons::json metadata file. Doing common json commands are not supported since the interface is mostly hidden behind the wrapper classes.
* Schema validation is pretty flimsy - not well thought out currently. It is forced during construction & during save time, and is forced to v1.2.6, with no options set/selected by the user. This will definitely cause issues during usage. At the same time, the wrapper classes currently don't enforce set-time schema validation, which should really be an option to avoid issues that only get reported once a dataset is written, causing a crash.

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
* There's a difference between "default" on load and "default" on write.
    - default on load: default value is assumed when the value is missing in a recording.
    - default on write: default value is assumed when a user does not specify a value to be written, and is substituted in during write-time.
* For non-conforming datasets, allow the user to define & extract ~header_bytes~ & trailing_bytes fields!
    - MVP-extract raw bytes & let the user mess with it (convert to data types, etc.).
* implement SigMF metadata loading, using json library. Interfaces should:
    - support SigMF Core Namespace.
    - support adding/setting/getting fields that are NOT defined by the core namespace.
    - support extension objects that are user-defined.
* May need to implement/expose JSONPath and JMESPath along with JSONPointer for user convenience...
    - maybe need to expose JSON object itself???
* ~For specification enforcement, may want to make base class for custom types to implement validation functionality (virtual function they all must implement).~
* SigMF Extensions should be user-extendable base classes - a user can implement a custom C++ class that defines an extension interface, for convenience.
    - same base class as for the global, capture, annotation, geolocation, and other fields???
* Ability for user-based extension JSON schemas to be added to validate portions of the JSON data.
* ~Implement supported versions as a const map where a specific version is mapped to a certain implementation of the Specification Validator - when a new spec drops, we simply inherit from the last implementation & re-implement functions that change.~
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
* Custom data types are not strictly enforced - instead, they are the default type specified by the Specification, with a custom/convenience wrapper class that validates on construction to provide a clean interface to interact with the field (ex: sigmf_io::Datatype). This way a user's (potentially incorrect) implementation may still load on the Metadata side, but will crash as expected when incorrect when you attempt to use facilities that require correctness.
* Does memory mapping do anything right now? - we create a vector in-memory and then load samples when the vector is initialized (doesn't this defeat the purpose)? need to double-check to see if this is even worth the effort/dependency...
    - If we end up implementing SigMFDataset with the iterable/vector interface, then we could make it work as a proxy to the memmapped dataset - only loading data into memory when actually indexed by the user. I think vectors can work like this already, so may just be a matter of pointing the vector to the correct spots on disk/intializing and returning one? I don't know... C++ ranges, etc.? Look into the proxy vector thing...
* Since offset is a SHOULD and not a SHALL, we need to account for when core:sample_start values are NOT offset by this value - we would need to ignore offset during data loading. This is a potential bug for people who make SigMFMetadata, define an offset value, BUT don't actually offset their capture and annotation core:sample_start values by this offset - this code would not load it properly unless they manually fix offset to be zero in the SigMFDataset. I'll have to think about how to handle this...
* Think of pattern to support MUST, MUST NOT, REQUIRED, SHALL, SHALL_NOT, SHOULD, SHOULD_NOT, RECOMMENDED, MAY, OPTIONAL as they come up for certain fields.
* allow for Meta-Only data reading.
* Something to figure out: Should this interface be for LOADING SigMFRecordings, or BUILDING SigMF Recordings? Or Both?
* If Both, we need to reconcile the problem of RAII - if things have to be "complete" to be initialized, then this may place a significant burden on the user when building a SigMF dataset - they will need to make sure they provide all correct & complete data in order to support the SigMF format. Maybe not that bad, we will see...
* Consider error handling after complete - need robust error messages to make SigMF dataset interactions easy/simple.
* Consider speed after complete - may require re-writing SigMFDataset interface to be template class in general to support the DataType (templatizing getSamples with DataTypes).This way we can have a new function to load data for every supported DataType, which would be optimal speed for data loading, rather than only having an interface for std::complex\<double>.
    - At this point, this means we need to write fast-path function overloads for specific cases - Conforming SigMFDatasets, one-channel datasets, the user loading the same type as what's on disk, etc.
* Question: Should this be thread-safe??? If we're dealing with threading, and people can build datasets as well as write to them, we could get race conditions for checking for computed values. Overall, could be messy.
* One option is to write optional header files that implement interfaces for certain common C++ libraries - Eigen, etc. - so that users of the library can optionally compile those interfaces for simpler usage, rather than converting to custom container formats for samples.
    - Also include common container interface operations (C++ ranges, .begin(), .end() interfaces, etc.) so that users can easily interface with a dataset themselves.
* API to allow a user to treat a NCD as a runtime SigMF dataset. You don't have to necessarily generate .sigmf-meta files, you can use the interface to describe only the needed information, then let the program run in memory.
* Extension Builder - make a tool to help add SigMF specification extensions for the project - a GUI interface lets you build the structure & requirements (data types, required vs. optional fields). This will enable groups to do the specifics of their work easily.
    * Also allow users to ingest an existing file & build a specification from it. This means reading a file, infering the data types, and suggesting the rules automatically. This will help with custom-made specs that were written first and defined rigidly later.
    * May also need to handle newly defined data types... YIKES.
* Update the interfaces for each class to implement different contruction options (copy, move, const, static, etc.) so that the user can make the most of each function.
* Maybe make SigMFDataType enums castable to strings, for error handling clarity...
* Make sure to handle inter-field specification requirements (ie. freq_upper_edge and freq_lower_edge can only exist if the other exists, otherwise it's invalid).
* Need to remember to handle versioning... both of this repo AND of SigMF versions.
* Currently data is returns raw from-disk. Consider the option to normalize the data for the user.
* Should consider in a later version, a facade/wrapper around jsoncons to abstract the json backend out - this way the user can decide to swap out the json backend OR I can swap it out if I think it matters later... for now, jsoncons is exposed in metadata constructors & when adding arbitrary fields.
