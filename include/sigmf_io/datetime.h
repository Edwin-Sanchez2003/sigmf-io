#ifndef Datetime_H
#define Datetime_H

#include <chrono>
#include <compare>
#include <string>
#include <regex>
#include <stdexcept>
#include <charconv>

namespace sigmf_io {

class Datetime {
public:
    explicit Datetime(const std::string& iso8601_utc);

    // get the standard library time_point corresponding to the date-time.
    std::chrono::sys_time<std::chrono::nanoseconds> time_point() const { return time_point_; }

    // get the raw string formatted version first read from the file.
    std::string to_string() const { return raw_; }

    // true if the source string encoded a leap second (":60"); the stored
    // time_point() is clamped to ":59" plus the fractional part, since
    // std::chrono::sys_time cannot represent a 61st second natively.
    bool is_leap_second() const { return is_leap_second_; }

    std::chrono::year  year()  const { return ymd().year(); }
    std::chrono::month month() const { return ymd().month(); }
    std::chrono::day   day()   const { return ymd().day(); }

    std::chrono::hours   hour()   const { return time_of_day().hours(); }
    std::chrono::minutes minute() const { return time_of_day().minutes(); }
    std::chrono::seconds second() const {
        return is_leap_second_ ? std::chrono::seconds{60} : time_of_day().seconds();
    }

    std::chrono::nanoseconds subsecond_nanoseconds() const { return time_of_day().subseconds(); }

    // Ordering/equality by instant, with leap-second status as a tiebreaker
    // so 23:59:60 and 23:59:59 (which share a time_point_) don't compare equal.
    // A leap second sorts after its clamped neighbor at the same time_point_.
    std::strong_ordering operator<=>(const Datetime& other) const {
        if (auto cmp = time_point_ <=> other.time_point_; cmp != 0) return cmp;
        return is_leap_second_ <=> other.is_leap_second_;
    }

    std::chrono::nanoseconds operator-(const Datetime& other) const {
        return time_point_ - other.time_point_;
    }

private:
    std::string raw_;
    bool is_leap_second_ = false;
    std::chrono::sys_time<std::chrono::nanoseconds> time_point_;

    std::chrono::year_month_day ymd() const {
        return std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(time_point_)};
    }

    std::chrono::hh_mm_ss<std::chrono::nanoseconds> time_of_day() const {
        using namespace std::chrono;
        return hh_mm_ss<nanoseconds>{time_point_ - floor<days>(time_point_)};
    }

    void parse_and_validate(const std::string& s);
};

} // end sigmf_io namespace

#endif // Datetime_H
