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

