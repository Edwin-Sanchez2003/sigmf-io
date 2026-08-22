include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

add_executable(sigmf_io_tests
    unit/sigmf_io/test_datatype.cpp
    unit/sigmf_io/test_dataset.cpp
    # ... add each test file as you write it

    integration/sigmf_io/simple_pipeline.cpp
)

target_link_libraries(sigmf_io_tests
    PRIVATE
        sigmf_io                  # your library — brings mio/jsoncons/includes along via PUBLIC
        Catch2::Catch2WithMain
)

# Optional: point tests at sample .sigmf files without hardcoding paths
target_compile_definitions(sigmf_io_tests
    PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

include(Catch)
catch_discover_tests(sigmf_io_tests)
