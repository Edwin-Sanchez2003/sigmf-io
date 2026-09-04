#ifndef SIGMF_IO_JSON_BASE_H
#define SIGMF_IO_JSON_BASE_H

#include <optional>
#include <fstream>
#include <filesystem>
#include <string>
#include <utility>

#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

// TODO: Consider json-style bracket interface for getting/setting values???

namespace sigmf_io {

class JSONBase
{
public:
    explicit JSONBase(const jsoncons::json& data);

    virtual ~JSONBase() = default;                  // needed for safe polymorphic destruction.

    // Will throw a jsoncons::json error if the value cannot be retrieved.
    template <typename T>
    T get(const std::string& json_pointer) const;

    // Will catch an error within std::optional, explicitly deferring missing values to the user.
    template <typename T>
    std::optional<T> get_optional(const std::string& json_pointer) const;

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

    // Escape hatch: direct read/write access to the underlying json value
    // at a given key or array index, bypassing the typed getter/setter API.
    // Mutating through the returned reference does not re-run validation —
    // callers doing so are responsible for leaving the object in a valid state.
    jsoncons::json& operator[](const std::string& key);
    const jsoncons::json& operator[](const std::string& key) const;

protected:
    JSONBase(jsoncons::json defaults, const jsoncons::json& overrides);
    jsoncons::json data_;
};

template <typename T>
T JSONBase::get(const std::string& json_pointer) const {
    return jsoncons::jsonpointer::get(this->data_, json_pointer).template as<T>();
}

template <typename T>
std::optional<T> JSONBase::get_optional(const std::string& json_pointer) const {
    try
    {
        return this->get<T>(json_pointer);
    }
    catch (const jsoncons::jsonpointer::jsonpointer_error&)
    {
        return std::nullopt; // key/path missing - pass this to user as an empty std::optional<T>.
    }
}

template <typename T>
void JSONBase::set(const std::string& json_pointer, T&& value) {
    jsoncons::jsonpointer::get(this->data_, json_pointer, true) = std::forward<T>(value);
}

inline jsoncons::json JSONBase::to_json() const {
    return data_;
}

} // end sigmf_io namespace

#endif // SIGMF_IO_JSON_BASE_H
