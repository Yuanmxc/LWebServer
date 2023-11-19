#ifndef DATA_H_
#define DATA_H_

#include <types.h>
#include <algorithm>
#include <string>

#include "../../base/copyable.h"

namespace ws {

class Date : public Copyable {
   public:
    struct YearMonthDay {
        int year;
        int month;
        int day;
    };

    static constexpr const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01;

    Date() : julianDayNumber_(0) {}

    Date(int year, int month, int day);

    explicit Date(int julianDayNum) : julianDayNumber_(julianDayNum) {}

    explicit Date(const struct tm&);

    void swap(Date& that) noexcept {
        std::swap(julianDayNumber_, that.julianDayNumber_);
    }

    bool valid() const noexcept { return julianDayNumber_ > 0; }

    std::string toIsoString() const;

    struct YearMonthDay yearMonthDay() const;

    int year() const noexcept { return yearMonthDay().year; }

    int month() const noexcept { return yearMonthDay().month; }

    int day() const noexcept { return yearMonthDay().day; }

    int weekDay() const noexcept {
        return (julianDayNumber_ + 1) % kDaysPerWeek;
    }

    int julianDayNumber() const noexcept { return julianDayNumber_; }

   private:
    int julianDayNumber_;
};

inline bool operator<(Date x, Date y) {
    return x.julianDayNumber() < y.julianDayNumber();
}

inline bool operator==(Date x, Date y) {
    return x.julianDayNumber() == y.julianDayNumber();
}

};  // namespace ws

#endif  // DATA_H_