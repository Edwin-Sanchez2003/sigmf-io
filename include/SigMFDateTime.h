#ifndef SIGMFDATETIME_H
#define SIGMFDATETIME_H

#include <chrono>
#include <string>
#include <regex>
#include <stdexcept>
#include <charconv>

class SigMFDateTime {
public:
    explicit SigMFDateTime(const std::string& iso8601_utc);

    // get the standard library time_point corresponding to the date-time.
    std::chrono::sys_time<std::chrono::nanoseconds> time_point() const { return time_point_; }

    // get the raw string formatted version first read from the file.
    std::string get_raw_datetime() const { return raw_; }

    // true if the source string encoded a leap second (":60"); the stored
    // time_point() is clamped to ":59" plus the fractional part, since
    // std::chrono::sys_time cannot represent a 61st second natively.
    bool is_leap_second() const { return is_leap_second_; }

private:
    std::string raw_;
    bool is_leap_second_ = false;
    std::chrono::sys_time<std::chrono::nanoseconds> time_point_;

    static std::chrono::sys_time<std::chrono::nanoseconds>
    parse_and_validate(const std::string& s, bool& leap_second_out);
};

#endif // SIGMFDATETIME_H
