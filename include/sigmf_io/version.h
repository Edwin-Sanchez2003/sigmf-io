#ifndef SIGMFVERSION_H
#define SIGMFVERSION_H

#include <string>

namespace sigmf_io {

class Version
{
public:
    explicit Version(const std::string& version);

    std::string to_string() const { return this->version_; }

    int major() const { return this->major_; }
    int minor() const { return this->minor_; }
    int patch() const { return this->patch_; }

private:
    std::string version_;
    int major_;
    int minor_;
    int patch_;
};

} // end sigmf_io namespace

#endif // SIGMFVERSION_H
