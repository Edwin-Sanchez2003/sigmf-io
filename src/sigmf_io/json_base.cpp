#include "sigmf_io/json_base.h"

#include <fstream>
#include <filesystem>
#include <string>
#include <utility>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>
#include <jsoncons_ext/mergepatch/mergepatch.hpp>

namespace sigmf_io {

JSONBase::JSONBase(jsoncons::json defaults, const jsoncons::json& overrides)
    : data_(std::move(defaults))
{
   this->merge_patch("", overrides);
}


void JSONBase::merge(const std::string& json_pointer, const jsoncons::json& value) {
    jsoncons::json& target = jsoncons::jsonpointer::get(this->data_, json_pointer, true);

    // no existing value in the json object - copy the value & return.
    if (target.is_null()) {
        target = value;
        return;
    }

    // trying to set an object with a scalar, or a scalar with an object - this does
    // not makes sense when merging..
    if (!target.is_object() || !value.is_object()) {
        throw std::invalid_argument(
            R"(merge(): both the existing value and the new value must be json objects.
             To completely overwrite an object/value (ignoring any existing values), call the 'set()' method instead.)");
    }

    target.merge(value);
}


void JSONBase::merge_patch(const std::string& json_pointer, const jsoncons::json& value) {
    jsoncons::json& target = jsoncons::jsonpointer::get(this->data_, json_pointer, true);

    // No existing value — just take the new value as-is (object or scalar).
    if (target.is_null()) {
        target = value;
        return;
    }

    if (!target.is_object() || !value.is_object()) {
        throw std::invalid_argument(
            R"(merge_patch(): both the existing value and the new value must be json objects.
             To completely overwrite an object/value (ignoring any existing values), call 'set()' instead.)");
    }

    jsoncons::mergepatch::apply_merge_patch(target, value);   // recursive: replaces/deletes only what `value` specifies
}


jsoncons::json& JSONBase::operator[](const std::string& key)
{
    return this->data_[key];   // auto-vivifies the key if absent, same as jsoncons::json::operator[]
}


const jsoncons::json& JSONBase::operator[](const std::string& key) const
{
    return this->data_.at(key);  // throws if absent — no auto-vivification on a const object
}

} // end sigmf_io namespace
