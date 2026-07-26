#include "SigMFDateTime.h"

#include <chrono>
#include <string>
#include <regex>
#include <stdexcept>
#include <charconv>

SigMFDateTime::SigMFDateTime(const std::string& iso8601_utc)
    : raw_(iso8601_utc),
      time_point_(parse_and_validate(iso8601_utc, is_leap_second_))
{}


std::chrono::sys_time<std::chrono::nanoseconds>
SigMFDateTime::parse_and_validate(const std::string& s, bool& leap_second_out)
{
    // Matches exactly: YYYY-MM-DDTHH:MM:SS[.fraction]Z
    // The literal 'Z' requirement means any other offset is rejected
    // by the pattern itself -- no separate offset check needed.
    static const std::regex pattern(
        R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(\.\d+)?Z$)");

    std::smatch m;
    if (!std::regex_match(s, m, pattern)) {
        throw std::runtime_error(
            "SigMFDateTime: '" + s + "' is not a valid SigMF ISO-8601 UTC timestamp");
    }

    const int year_val   = std::stoi(m[1]);
    const unsigned month_val = static_cast<unsigned>(std::stoi(m[2]));
    const unsigned day_val   = static_cast<unsigned>(std::stoi(m[3]));
    const int hour   = std::stoi(m[4]);
    const int minute = std::stoi(m[5]);
    const int second = std::stoi(m[6]);

    using namespace std::chrono;

    const year_month_day ymd{year{year_val}, month{month_val}, day{day_val}};
    if (!ymd.ok()) {
        throw std::runtime_error(
            "SigMFDateTime: '" + s + "' has a calendar date that does not exist");
    }
    if (hour > 23) {
        throw std::runtime_error("SigMFDateTime: '" + s + "' has an invalid hour");
    }
    if (minute > 59) {
        throw std::runtime_error("SigMFDateTime: '" + s + "' has an invalid minute");
    }
    if (second > 60) { // 60 permitted only for a leap second
        throw std::runtime_error("SigMFDateTime: '" + s + "' has an invalid second");
    }
    leap_second_out = (second == 60);

    // Fractional seconds: pad/truncate to nanosecond precision (9 digits).
    // Digits beyond nanosecond resolution are silently dropped -- documented
    // limitation, since std::chrono has no arbitrary-precision duration type.
    nanoseconds frac{0};
    if (m[7].matched) {
        std::string digits = m[7].str().substr(1); // drop leading '.'
        digits.resize(9, '0');
        long long ns_value = 0;
        std::from_chars(digits.data(), digits.data() + 9, ns_value);
        frac = nanoseconds{ns_value};
    }

    const int representable_second = leap_second_out ? 59 : second;
    const sys_days days = sys_days{ymd};

    return days + hours{hour} + minutes{minute} + seconds{representable_second} + frac;
}
