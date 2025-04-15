/********************************************************************************
* @author : hexne
* @date   : 2025/04/14 22:41:01
********************************************************************************/

module;
#include <vector>
#include <ranges>
#include <chrono>
export module Calendar;

import Date;

export class Calendar {
    using Month = std::vector<Date>;
    using Year = std::vector<Month>;
    std::vector<Year> calendar_;
public:
    Date &find(const Date &date) {
        for (auto &day : calendar_ | std::ranges::views::join | std::ranges::views::join) {
            if (date == day)
                return day;
        }
        throw std::runtime_error("This Day found");
    }

    Calendar() {
        Date end_time = std::chrono::system_clock::now();

        for (int year = 2023; year <= end_time.year; year++) {
            Year push_year;
            for (int month = -1; month < 12; month++) {
                Month push_month;
                if (year == end_time.year && month > end_time.month)
                    break;
                for (int day = -1; day <= Date::count_month_day(year, month); day++) {
                    if (year == end_time.year && month == end_time.month && day > end_time.day)
                        break;
                    Date date {year, month, day};
                    push_month.push_back(date);
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