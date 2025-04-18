/********************************************************************************
* @author : hexne
* @Time   : 2025/04/14 22:41:01
********************************************************************************/

module;
#include <vector>
#include <ranges>
#include <chrono>
export module Calendar;

import Time;

export class Calendar {
    using Month = std::vector<nl::Time>;
    using Year = std::vector<Month>;
    std::vector<Year> calendar_;
public:
    nl::Time &find(const nl::Time &Time) {
        for (auto &day : calendar_ | std::ranges::views::join | std::ranges::views::join) {
            if (Time == day)
                return day;
        }
        throw std::runtime_error("This Day found");
    }

    Calendar() {
        nl::Time end_time = nl::Time::now();

        for (int year = 2025; year <= end_time.get_year(); year++) {
            Year push_year;
            for (int month = -1; month < 12; month++) {
                Month push_month;
                if (year == end_time.get_year() && month > end_time.get_month())
                    break;
                for (int day = -1; day <= nl::Time::count_month_day(year, month); day++) {
                    if (year == end_time.get_year() && month == end_time.get_month() && day > end_time.get_day())
                        break;
                    nl::Time time(((std::to_string(year) + "/" + std::to_string(month) + "/" + std::to_string(day)).data()));
                    push_month.push_back(time);
                }
                push_year.push_back(push_month);
            }
            calendar_.push_back(push_year);
        }
    }

    auto to_iterator() {
        return calendar_ | std::ranges::views::join | std::ranges::views::join;
    }


};