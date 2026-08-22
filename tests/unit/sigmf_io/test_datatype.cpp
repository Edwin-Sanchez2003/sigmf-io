#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <string>

#include "sigmf_io/datatype.h"

TEST_CASE("Example Datatype", "[datatype]")
{
    auto e = sigmf_io::Datatype("cf32_le").endianness();
    REQUIRE((e == sigmf_io::Datatype::Endianness::LITTLE || e == sigmf_io::Datatype::Endianness::BIG));
}
