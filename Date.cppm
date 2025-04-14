

module;
#include <chrono>
#include <vector>
#include <ranges>
#include <memory>
#include <algorithm>
export module Date;

import Plan;

export struct Date {
    int year{}, month{}, day{};
    std::vector<std::shared_ptr<Plan>> done_plans;  // 每日完成的计划列表

    Date(int y, int m, int d) : year(y), month(m), day(d) {  }
    Date(std::chrono::time_point<std::chrono::system_clock> time_point) {
        auto ymd = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(time_point)};
        year = ymd.year().operator int();
        month = ymd.month().operator unsigned();
        day = ymd.day().operator unsigned();
    }
    bool operator==(const Date &other) const {
        return day == other.day && month == other.month && year == other.year;
    }

    bool find(const Plan &find_plan) const {
        return std::ranges::any_of(done_plans, [&find_plan](const auto &plan) {
            return *plan == find_plan;
        });
    }

    static int count_month_day(int year, int month) {
        if (month == 2) {
            return (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0) ? 29 : 28;
        }
        constexpr int days[] = {0, 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return days[month];
    }
};

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

        for (int year = 2025; year <= end_time.year; year++) {
            Year push_year;
            for (int month = 1; month < 12; month++) {
                Month push_month;
                if (year == end_time.year && month > end_time.month)
                    break;
                for (int day = 1; day <= Date::count_month_day(year, month); day++) {
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


};
