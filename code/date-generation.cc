#include <cstdint>

#include <print>

#include "bits/Date.h"

int main()
{
  fw::Month month = fw::Month::Jan;

  for (uint8_t i = 0; i < fw::MonthsInYear; ++i) {
    uint8_t days = fw::days_in_month(month);

    for (uint8_t day = 1; day <= days; ++day) {
      const fw::HourMinuteSeconds hms_sunrise = fw::compute_sunrise({ .month = month, .day = day });
      const fw::Date date_sunrise = {
        .year = 0,
        .month = month,
        .day = day,
        .weekday = fw::WeekDay::Mon,
        .hours = hms_sunrise.hours,
        .minutes = hms_sunrise.minutes,
        .seconds = hms_sunrise.seconds
      };

      std::println("Sunrise: {}", date_sunrise.to_string());

      const fw::HourMinuteSeconds hms_sunset = fw::compute_sunset({ .month = month, .day = day });
      const fw::Date date_sunset = {
        .year = 0,
        .month = month,
        .day = day,
        .weekday = fw::WeekDay::Mon,
        .hours = hms_sunset.hours,
        .minutes = hms_sunset.minutes,
        .seconds = hms_sunset.seconds
      };

      std::println("Sunset:  {}", date_sunset.to_string());
    }

    month = fw::next_month(month);
  }

}
