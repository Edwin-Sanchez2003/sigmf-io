#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <string>

#include <jsoncons/json.hpp>

#include "sigmf_io/metadata.h"
#include "sigmf_io/global.h"
#include "sigmf_io/capture.h"
#include "sigmf_io/annotation.h"

namespace {

// Minimal, schema-valid SigMF metadata used as a fixture across cases below.
// Adjust the required-field set here if your static/sigmf-schema.json differs.
jsoncons::json make_valid_metadata_json()
{
    return jsoncons::json::parse(R"(
    {
        "global": {
            "core:datatype": "cf32_le",
            "core:version": "1.2.6",
            "core:sample_rate": 1000000
        },
        "captures": [
            { "core:sample_start": 0 }
        ],
        "annotations": [
            { "core:sample_start": 0, "core:sample_count": 100, "core:label": "burst" }
        ]
    }
    )");
}

} // anonymous namespace


TEST_CASE("Metadata constructs from a fully populated json object", "[metadata]")
{
    sigmf_io::Metadata meta(make_valid_metadata_json());

    REQUIRE(meta.global.datatype() == "cf32_le");
    REQUIRE(meta.global.sample_rate().has_value());
    REQUIRE(meta.global.sample_rate().value() == 1000000);

    REQUIRE(meta.captures.size() == 1);
    REQUIRE(meta.captures.at(0).sample_start() == 0);

    REQUIRE(meta.annotations.size() == 1);
    REQUIRE(meta.annotations.at(0).sample_start() == 0);
    REQUIRE(meta.annotations.at(0).sample_count().has_value());
    REQUIRE(meta.annotations.at(0).sample_count().value() == 100);
}


TEST_CASE("Metadata defaults missing captures and annotations to empty vectors", "[metadata]")
{
    jsoncons::json j(jsoncons::json_object_arg);
    j.insert_or_assign("global", make_valid_metadata_json()["global"]);
    // "captures" and "annotations" intentionally omitted.

    sigmf_io::Metadata meta(j);

    REQUIRE(meta.captures.empty());
    REQUIRE(meta.annotations.empty());
    REQUIRE(meta.global.datatype() == "cf32_le");
}


TEST_CASE("Metadata::to_json round-trips through parsing", "[metadata]")
{
    jsoncons::json original = make_valid_metadata_json();
    sigmf_io::Metadata meta(original);

    jsoncons::json dumped = meta.to_json();

    REQUIRE(dumped["global"]["core:datatype"].as<std::string>() == "cf32_le");
    REQUIRE(dumped["captures"].size() == 1);
    REQUIRE(dumped["captures"][0]["core:sample_start"].as<int64_t>() == 0);
    REQUIRE(dumped["annotations"].size() == 1);
    REQUIRE(dumped["annotations"][0]["core:label"].as<std::string>() == "burst");

    // Re-constructing from the dump should reproduce the same object.
    sigmf_io::Metadata reloaded(dumped);
    REQUIRE(reloaded.captures.size() == meta.captures.size());
    REQUIRE(reloaded.annotations.size() == meta.annotations.size());
}


TEST_CASE("Metadata converts via jsoncons json_conv_traits (.is / .as)", "[metadata][traits]")
{
    jsoncons::json j = make_valid_metadata_json();

    REQUIRE(j.is<sigmf_io::Metadata>());

    sigmf_io::Metadata meta = j.as<sigmf_io::Metadata>();
    REQUIRE(meta.global.datatype() == "cf32_le");
    REQUIRE(meta.captures.size() == 1);

    // An object missing the required "global" key should not report as convertible.
    jsoncons::json not_metadata(jsoncons::json_object_arg);
    not_metadata.insert_or_assign("foo", "bar");
    REQUIRE_FALSE(not_metadata.is<sigmf_io::Metadata>());
}


TEST_CASE("Metadata assigns into a jsoncons::json value via json_conv_traits", "[metadata][traits]")
{
    sigmf_io::Metadata meta(make_valid_metadata_json());

    jsoncons::json wrapper(jsoncons::json_object_arg);
    wrapper.insert_or_assign("metadata", meta); // uses json_conv_traits<Json, Metadata>::to_json

    REQUIRE(wrapper["metadata"]["global"]["core:datatype"].as<std::string>() == "cf32_le");
    REQUIRE(wrapper["metadata"]["captures"].size() == 1);
}


TEST_CASE("Metadata::save writes a reloadable .sigmf-meta file", "[metadata][io]")
{
    sigmf_io::Metadata meta(make_valid_metadata_json());

    std::filesystem::path out_path =
        std::filesystem::temp_directory_path() / "sigmf_io_test_metadata.sigmf-meta";

    // Clean up any leftovers from a previous failed run before asserting overwrite=false behavior.
    std::filesystem::remove(out_path);

    meta.save(out_path.string(), /*overwrite=*/false);
    REQUIRE(std::filesystem::exists(out_path));

    // overwrite=false against an existing file should now throw.
    REQUIRE_THROWS_AS(meta.save(out_path.string(), /*overwrite=*/false), std::runtime_error);

    // overwrite=true should succeed.
    REQUIRE_NOTHROW(meta.save(out_path.string(), /*overwrite=*/true));

    sigmf_io::Metadata reloaded(out_path.string());
    REQUIRE(reloaded.global.datatype() == meta.global.datatype());
    REQUIRE(reloaded.captures.size() == meta.captures.size());

    std::filesystem::remove(out_path);
}


TEST_CASE("JSONBase bracket operator provides a raw escape hatch", "[metadata][json_base]")
{
    sigmf_io::Metadata meta(make_valid_metadata_json());

    // Set a field with no dedicated typed setter yet, through the bracket escape hatch.
    meta.global["core:hw"] = "Test SDR v1";
    REQUIRE(meta.global["core:hw"].as<std::string>() == "Test SDR v1");
    REQUIRE(meta.global.hw().has_value());
    REQUIRE(meta.global.hw().value() == "Test SDR v1");

    // const access should throw for a genuinely absent key rather than auto-vivify.
    const sigmf_io::Global& const_global = meta.global;
    REQUIRE_THROWS(const_global["core:does_not_exist"]);
}
