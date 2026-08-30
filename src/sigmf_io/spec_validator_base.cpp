#include "sigmf_io/spec_validator_base.h"

#include <string>
#include <iostream>
#include <sstream>
#include <expected>

#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonschema/jsonschema.hpp>

namespace sigmf_io {

jsoncons::jsonschema::json_schema<jsoncons::json> SpecValidatorBase::make_validator()
{
    jsoncons::json schema_json = jsoncons::json::parse(sigmf_io::SIGMF_SCHEMA_JSON);
    return jsoncons::jsonschema::make_json_schema(std::move(schema_json));
}


void SpecValidatorBase::accumulate(std::vector<std::string>& errors, const std::expected<void, std::string>& result)
{
    if (!result)
        errors.push_back(result.error());
}


void SpecValidatorBase::raise_errors(const std::expected<void, std::vector<std::string>>& validation_result)
{
    if (!validation_result.has_value()) // "this expected does NOT hold a success — it holds an error"
    {
        const std::vector<std::string>& messages = validation_result.error();

        for (const auto& msg : messages)
        {
            std::cerr << msg << '\n';
        }

        // Combine into one string for the exception, if you want a single message
        std::ostringstream oss;
        for (const auto& msg : messages)
        {
            oss << msg << '\n';
        }

        throw std::runtime_error(oss.str());
    }
}

} // end sigmf_io namespace
