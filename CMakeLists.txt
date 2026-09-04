cmake_minimum_required(VERSION 3.25)
project(sigmf_io VERSION 0.1.0 LANGUAGES CXX)
include(GNUInstallDirs)

# Template header to track versioning of sigmf_io for library users.
configure_file(
    ${CMAKE_SOURCE_DIR}/include/sigmf_io/lib_version.h.in
    ${CMAKE_BINARY_DIR}/include/sigmf_io/lib_version.h
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ── FetchContent for mio ──────────────────────────────────────────────
FetchContent_Declare(
    mio
    GIT_REPOSITORY https://github.com/vimpunk/mio.git
    GIT_TAG        8b6b7d878c89e81614d05edca7936de41ccdd2da
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(mio)

# ── FetchContent for jsoncons ─────────────────────────────────────────
FetchContent_Declare(
    jsoncons
    GIT_REPOSITORY https://github.com/danielaparker/jsoncons.git
    GIT_TAG        d35782026b6208d7c0ed80f5a8165a7a3b868e2f
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(jsoncons)

# ── Embed the SigMF schema as a generated header ──────────────────
set(SIGMF_SCHEMA_SRC ${CMAKE_SOURCE_DIR}/static/sigmf-schema.json)
set(SIGMF_SCHEMA_GEN ${CMAKE_BINARY_DIR}/include/sigmf_io/sigmf_schema_data.h)

add_custom_command(
    OUTPUT ${SIGMF_SCHEMA_GEN}
    COMMAND ${CMAKE_COMMAND}
            -DIN_FILE=${SIGMF_SCHEMA_SRC}
            -DOUT_FILE=${SIGMF_SCHEMA_GEN}
            -DVAR_NAME=SIGMF_SCHEMA_JSON
            -P ${CMAKE_SOURCE_DIR}/cmake/EmbedFile.cmake
    DEPENDS ${SIGMF_SCHEMA_SRC} ${CMAKE_SOURCE_DIR}/cmake/EmbedFile.cmake
    COMMENT "Embedding sigmf-schema.json into header"
)

add_custom_target(sigmf_schema_header DEPENDS ${SIGMF_SCHEMA_GEN})

# ── sigmf_io library ──────────────────────────────────────────────────
add_library(sigmf_io STATIC
    include/sigmf_io/version.h     src/sigmf_io/version.cpp
    include/sigmf_io/datatype.h    src/sigmf_io/datatype.cpp
    include/sigmf_io/datetime.h    src/sigmf_io/datetime.cpp
    include/sigmf_io/uuid.h        src/sigmf_io/uuid.cpp
    include/sigmf_io/sha512.h      src/sigmf_io/sha512.cpp
    include/sigmf_io/json_base.h   src/sigmf_io/json_base.cpp
    include/sigmf_io/global.h      src/sigmf_io/global.cpp
    include/sigmf_io/capture.h     src/sigmf_io/capture.cpp
    include/sigmf_io/annotation.h  src/sigmf_io/annotation.cpp
    include/sigmf_io/geolocation.h src/sigmf_io/geolocation.cpp
    include/sigmf_io/metadata.h    src/sigmf_io/metadata.cpp
    include/sigmf_io/dataset.h     src/sigmf_io/dataset.cpp
    include/sigmf_io/recording.h   src/sigmf_io/recording.cpp
    include/sigmf_io/spec_validator_base.h
    include/sigmf_io/v1_2_6/spec_validator.h src/sigmf_io/v1_2_6/spec_validator.cpp
    src/sigmf_io/spec_validator_base.cpp
    include/sigmf_io/capture_traits.h
    include/sigmf_io/annotation_traits.h
    include/sigmf_io/global_traits.h
    include/sigmf_io/metadata_traits.h
)

add_dependencies(sigmf_io sigmf_schema_header)

target_link_libraries(sigmf_io
    PUBLIC mio::mio
           jsoncons
)

target_include_directories(sigmf_io
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ── Tests ──────────────────────────────────────────────────────────────
option(BUILD_TESTS "Build the test suite" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ── Install ─────────────────────────────────────────────────────────────
install(TARGETS sigmf_io
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/sigmf_io
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES ${CMAKE_BINARY_DIR}/include/sigmf_io/lib_version.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sigmf_io
)
