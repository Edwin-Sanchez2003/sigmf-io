#include "sigmf_io/datetime.h"

#include <chrono>
#include <string>
#include <regex>
#include <stdexcept>
#include <charconv>

namespace sigmf_io {

Datetime::Datetime(const std::string& iso8601_utc)
    : raw_(iso8601_utc)
{
    parse_and_validate(iso8601_utc);
}

void Datetime::parse_and_validate(const std::string& s)
{
    static const std::regex pattern(
        R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(\.\d+)?Z$)");
    std::smatch m;
    if (!std::regex_match(s, m, pattern)) {
        throw std::invalid_argument(
            "Datetime: '" + s + "' is not a valid SigMF ISO-8601 UTC timestamp");
    }
    const int year_val       = std::stoi(m[1]);
    const unsigned month_val = static_cast<unsigned>(std::stoi(m[2]));
    const unsigned day_val   = static_cast<unsigned>(std::stoi(m[3]));
    const int hour   = std::stoi(m[4]);
    const int minute = std::stoi(m[5]);
    const int second = std::stoi(m[6]);

    using namespace std::chrono;
    const year_month_day parsed_ymd{
        std::chrono::year{year_val},
        std::chrono::month{month_val},
        std::chrono::day{day_val}
    };
    if (!parsed_ymd.ok()) {
        throw std::invalid_argument(
            "Datetime: '" + s + "' has a calendar date that does not exist");
    }
    if (hour > 23)   throw std::invalid_argument("Datetime: '" + s + "' has an invalid hour");
    if (minute > 59) throw std::invalid_argument("Datetime: '" + s + "' has an invalid minute");
    if (second > 60) throw std::invalid_argument("Datetime: '" + s + "' has an invalid second");

    is_leap_second_ = (second == 60);
    if (is_leap_second_ && !(hour == 23 && minute == 59)) {
        throw std::invalid_argument(
            "Datetime: '" + s + "' has a leap second outside of 23:59:60");
    }

    nanoseconds frac{0};
    if (m[7].matched) {
        std::string digits = m[7].str().substr(1);
        digits.resize(9, '0');
        long long ns_value = 0;
        std::from_chars(digits.data(), digits.data() + 9, ns_value);
        frac = nanoseconds{ns_value};
    }

    const int representable_second = is_leap_second_ ? 59 : second;
    const sys_days days = sys_days{parsed_ymd};
    time_point_ = days + hours{hour} + minutes{minute} + seconds{representable_second} + frac;
}

} // end sigmf_io namespace
