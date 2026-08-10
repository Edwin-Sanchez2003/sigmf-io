#ifndef JSON_BASE_H
#define JSON_BASE_H

#include <fstream>
#include <filesystem>
#include <string>
#include <utility>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

// TODO: Consider json-style bracket interface for getting/setting values???

class JSONBase
{
public:
    JSONBase();
    explicit JSONBase(const jsoncons::json& data);
    explicit JSONBase(jsoncons::json&& data);       // avoids a copy when constructing from a temporary json object.
    virtual ~JSONBase() = default;                  // needed for safe polymorphic destruction.

    template <typename T>
    T get(const std::string& json_pointer) const;

    template <typename T>
    void set(const std::string& json_pointer, T&& value);

    // Shallow merge: adds keys from value that don't already exist in the target object.
    // Existing keys (including nested objects) are left untouched — no recursion.
    void merge(const std::string& json_pointer, const jsoncons::json& value);

    // Recursive merge patch (RFC 7396): keys present in value overwrite the target
    // at any depth, recursing into nested objects rather than replacing them wholesale.
    // Keys not present in value are left untouched. A null in value deletes that key
    // from the target rather than setting it to JSON null.
    void merge_patch(const std::string& json_pointer, const jsoncons::json& value);

    virtual jsoncons::json to_json() const;

private:
    jsoncons::json data_;
};

template <typename T>
T JSONBase::get(const std::string& json_pointer) const {
    return jsoncons::jsonpointer::get(this->data_, json_pointer).template as<T>();
}

template <typename T>
void JSONBase::set(const std::string& json_pointer, T&& value) {
    jsoncons::jsonpointer::get(this->data_, json_pointer, true) = std::forward<T>(value);
}

inline jsoncons::json JSONBase::to_json() const {
    return data_;
}

#endif // JSON_BASE_H
