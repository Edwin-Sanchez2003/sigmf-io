#ifndef SIGMFRECORDING_H
#define SIGMFRECORDING_H

class SigMFRecording
{
public:
    SigMFRecording(std::string file_path);

    std::string file_path() const { return this->file_path_; }

private:
    std::string file_path_;
};

#endif // SIGMFRECORDING_H
