/********************************************************************************
* @author : hexne
* @Time   : 2025/04/14 22:41:01
********************************************************************************/

module;
#include <vector>
#include <ranges>
#include <chrono>
#include <memory>
#include <map>
export module Calendar;

import Time;
import Plan;

export extern "C++" class Calendar {
    using Date = std::pair<nl::Time, std::vector<std::shared_ptr<Plan>>>;
    using Month = std::vector<Date>;
    using Year = std::vector<Month>;
    std::vector<Year> calendar_;
public:

    Calendar() {
		nl::Time end_date = nl::Time::now();

        for (int year = 2025; year <= end_date.get_year(); year++) {
            Year push_year;
            for (int month = 1; month <= 12; month++) {
                Month push_month;
                if (year == end_date.get_year() && month > end_date.get_month())
                    break;
                for (int day = 1; day <= nl::Time::count_month_day(year, month); day++) {
                    if (year == end_date.get_year() && month == end_date.get_month() && day > end_date.get_day())
                        break;
                    nl::Time time(((std::to_string(year) + "/" + std::to_string(month) + "/" + std::to_string(day)).data()));
                    push_month.push_back({ time, {} });
                }
                push_year.push_back(push_month);
            }
            calendar_.push_back(push_year);
        }
    }

    auto to_iterator() {
        return calendar_ | std::ranges::views::join | std::ranges::views::join;
    }

	Date& operator[](size_t year, size_t month, size_t day) {
		return calendar_[year - 2025][month][day];
	}

};