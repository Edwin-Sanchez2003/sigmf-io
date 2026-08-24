#include <catch2/catch_test_macros.hpp>

#include <jsoncons/json.hpp>

#include <iostream>
#include <string>

#include "sigmf_io/global.h"

TEST_CASE("Default Initializer", "[global]")
{
    sigmf_io::Global global;
    std::cout << "Default Global: " << global.to_json() << '\n';
    SUCCEED("Constructed and serialized without throwing");
}
