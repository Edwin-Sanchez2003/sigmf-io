#ifndef SIGMFMETADATA_H
#define SIGMFMETADATA_H

#include <fstream>
#include <filesystem>
#include <string>
#include <utility>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

class SigMFMetadata
{
public:
    SigMFMetadata();
    explicit SigMFMetadata(const std::string& meta_path);
    explicit SigMFMetadata(const jsoncons::json& meta);

    std::string meta_path() const { return this->meta_path_; }

    template <typename T>
    T get(const std::string& json_pointer) const;

    template <typename T>
    void set(const std::string& json_pointer, T&& value);

    void save(const std::string& file_path, bool overwrite = false);

private:
    static inline constexpr std::string META_EXT = ".sigmf-meta";
    std::string meta_path_;
    jsoncons::json meta_;

    bool ends_with(const std::string& value, const std::string& ending) const;
};

template <typename T>
T SigMFMetadata::get(const std::string& json_pointer) const {
    return jsoncons::jsonpointer::get(this->meta_, json_pointer).template as<T>();
}

template <typename T>
void SigMFMetadata::set(const std::string& json_pointer, T&& value) {
    jsoncons::jsonpointer::get(this->meta_, json_pointer, true) = std::forward<T>(value);
}

#endif // SIGMFMETADATA_H
