#ifndef SIGMFVERSION_H
#define SIGMFVERSION_H

#include <string>

class SigMFVersion
{
public:
    explicit SigMFVersion(const std::string& version);

    std::string version() const { return this->version_; }

    int major() const { return this->major_; }
    int minor() const { return this->minor_; }
    int patch() const { return this->patch_; }

private:
    std::string version_;
    int major_;
    int minor_;
    int patch_;
};

#endif // SIGMFVERSION_H
