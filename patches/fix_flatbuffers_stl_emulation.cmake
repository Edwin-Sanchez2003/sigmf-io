# Patch stl_emulation.h to fix const member assignment error on newer GCC
set(_file "${CMAKE_BINARY_DIR}/_deps/libsigmf-src/external/flatbuffers/include/flatbuffers/stl_emulation.h")

if(EXISTS ${_file})
    file(READ ${_file} _contents)
    string(REPLACE
        "count_ = other.count_;"
        "// count_ = other.count_;  // patched: const member, not assignable"
        _contents "${_contents}")
    file(WRITE ${_file} "${_contents}")
    message(STATUS "Patched flatbuffers/stl_emulation.h")
endif()
