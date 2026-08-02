#ifndef SIGMFMETADATA_H
#define SIGMFMETADATA_H

#include <string>
#include <jsoncons/json.hpp>

class SigMFMetadata
{
public:
    explicit SigMFMetadata();
    SigMFMetadata(const std::string& meta_path);
    SigMFMetadata(const jsoncons::json& meta);

    std::string meta_path() const { return this->meta_path_; }

    template <typename T>
    T get(const std::string& json_pointer) const;

    template <typename T>
    void set(const std::string& json_pointer, T&& value);

    void save(const std::string& file_path, bool overwrite = false);

private:
    std::string meta_path_;
    jsoncons::json meta_;

    bool ends_with(const std::string& value, const std::string& ending) const;
};

#endif // SIGMFMETADATA_H
