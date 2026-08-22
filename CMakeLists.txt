cmake_minimum_required(VERSION 3.25)

cmake_policy(SET CMP0048 NEW)  # project() manages VERSION variables
cmake_policy(SET CMP0071 NEW)  # AUTOMOC/AUTOUIC skip GENERATED files cleanly

project(BurstRFBackend VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)              # libsigmf v1.0 requires C++17 for std::optional
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ── FetchContent for mio (cross-platform memory mapping) ─────────────────────
FetchContent_Declare(
    mio
    GIT_REPOSITORY https://github.com/vimpunk/mio.git
    GIT_TAG        8b6b7d878c89e81614d05edca7936de41ccdd2da # Pin to a commit hash.
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(mio)
# ─────────────────────────────────────────────────────────────────────────────

# ── FetchContent for jsoncons (header-only JSON parsing/schema library) ──────
FetchContent_Declare(
    jsoncons
    GIT_REPOSITORY https://github.com/danielaparker/jsoncons.git
    GIT_TAG        d35782026b6208d7c0ed80f5a8165a7a3b868e2f     # v1.8.1 - June 9th, 2026
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(jsoncons)
# ─────────────────────────────────────────────────────────────────────────────

add_executable(BurstRFBackend src/main.cpp
    include/sigmf_io/version.h src/sigmf_io/version.cpp
    include/sigmf_io/datatype.h src/sigmf_io/datatype.cpp
    include/sigmf_io/datetime.h src/sigmf_io/datetime.cpp
    include/sigmf_io/uuid.h src/sigmf_io/uuid.cpp
    include/sigmf_io/sha512.h src/sigmf_io/sha512.cpp

    include/sigmf_io/json_base.h src/sigmf_io/json_base.cpp
    include/sigmf_io/global.h src/sigmf_io/global.cpp
    include/sigmf_io/capture.h src/sigmf_io/capture.cpp
    include/sigmf_io/annotation.h src/sigmf_io/annotation.cpp
    include/sigmf_io/geolocation.h src/sigmf_io/geolocation.cpp

    include/sigmf_io/metadata.h src/sigmf_io/metadata.cpp
    include/sigmf_io/dataset.h src/sigmf_io/dataset.cpp
    include/sigmf_io/recording.h src/sigmf_io/recording.cpp
)

target_link_libraries(BurstRFBackend
    PRIVATE mio::mio                # mio for cross-platform memory mapping
            jsoncons                # jsoncons for handling JSON data
)

# libsigmf generates headers into the build tree; this makes them findable
target_include_directories(BurstRFBackend
    PRIVATE
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
)

include(GNUInstallDirs)
install(TARGETS BurstRFBackend
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
