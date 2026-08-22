#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <string>

#include "sigmf_io/dataset.h"

TEST_CASE("Example Dataset", "[dataset]")
{
    sigmf_io::Dataset ds("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-data", sigmf_io::Datatype("cf32_le"), 1, 0);
    REQUIRE(ds.datatype().to_string() == sigmf_io::Datatype("cf32_le").to_string());       // e.g. a fresh Dataset starts empty
}
