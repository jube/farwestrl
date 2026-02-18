#ifndef FW_DATE_H
#define FW_DATE_H

#include <cstdint>

#include <string>

#include <gf2/core/Random.h>
#include <gf2/core/TypeTraits.h>

namespace fw {

  constexpr uint16_t SecondsInMinute = 60;
  constexpr uint16_t MinutesInHour = 60;
  constexpr uint8_t HoursInDay = 24;
  constexpr uint8_t DaysInWeek = 7;
  constexpr uint8_t MonthsInYear = 12;

  constexpr uint8_t DaysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  constexpr uint32_t DaysInYear = std::accumulate(std::begin(DaysInMonth), std::end(DaysInMonth), 0u);

  enum class WeekDay : uint8_t {
    Mon,
    Tue,
    Wed,
    Thu,
    Fri,
    Sat,
    Sun,
  };

  constexpr WeekDay next_weekday(WeekDay weekday)
  {
    return WeekDay{uint8_t((uint8_t(weekday) + 1) % DaysInWeek)};
  }

  enum class Month : uint8_t {
    Jan,
    Feb,
    Mar,
    Apr,
    Jun,
    Jul,
    Aug,
    Sep,
    Oct,
    Nov,
    Dec,
  };

  constexpr uint8_t days_in_month(Month month)
  {
    return DaysInMonth[uint8_t(month)];
  }

  constexpr Month next_month(Month month)
  {
    return Month{uint8_t((uint8_t(month) + 1) % MonthsInYear)};
  }

  enum class Phase {
    Dawn,
    Morning,
    Noon,
    Afternoon,
    Dusk,
    Night,
  };

  struct Date {
    uint8_t year;
    Month month;
    uint8_t day;
    WeekDay weekday;
    uint8_t hours;
    uint16_t minutes;
    uint16_t seconds;

    std::string to_string() const;
    std::string to_string_hours_minutes() const;

    void add_seconds(uint16_t duration_in_seconds);

    Phase phase() const;

    static Date generate_random(gf::Random* random);
  };

  bool operator<(const Date& lhs, const Date& rhs);
  bool operator==(const Date& lhs, const Date& rhs);

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<Date, Archive>& date)
  {
    return ar | date.year | date.month | date.day | date.weekday | date.hours | date.minutes | date.seconds;
  }

  struct MonthDay {
    Month month;
    uint8_t day;
  };

  MonthDay generate_random_birthday(gf::Random* random);

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<MonthDay, Archive>& month_day)
  {
    return ar | month_day.month | month_day.day;
  }

  struct HourMinuteSeconds {
    uint8_t hours;
    uint16_t minutes;
    uint16_t seconds;

    std::string to_string() const;
  };

  HourMinuteSeconds compute_sunrise(const MonthDay& month_day);
  HourMinuteSeconds compute_sunset(const MonthDay& month_day);

}

#endif // FW_DATE_H
