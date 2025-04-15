/*******************************************************************************
* @author : hexne
* @date   : 2025/4/14 15:24:32
* @brief  : 日历中的每一天
*******************************************************************************/

module;
#include <chrono>
#include <vector>
#include <ranges>
#include <memory>
#include <algorithm>
export module Date;

export struct Date {
    int year{}, month{}, day{}, week{}, day_count{};

    Date() = default;

    Date(int y, int m, int d) : year(y), month(m), day(d) {
        namespace sc = std::chrono;
        auto ymd = sc::year_month_day(sc::year(y), sc::month(m), sc::day(d));
        day_count = ymd.operator sc::sys_days().time_since_epoch().count();
        week = std::chrono::weekday(ymd).iso_encoding();
    }
    Date(std::chrono::time_point<std::chrono::system_clock> time_point) {
        auto ymd = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(time_point)};
        year = ymd.year().operator int();
        month = ymd.month().operator unsigned();
        day = ymd.day().operator unsigned();
        day_count = ymd.operator std::chrono::sys_days().time_since_epoch().count();
        week = std::chrono::weekday(ymd).iso_encoding();
    }

    auto operator <=> (const Date &other) const {
        return std::strong_order(day_count, other.day_count);
    }
    bool operator == (const Date &other) const {
        return day_count == other.day_count;
    }
    int operator - (const Date &other) const {
        return day_count - other.day_count;
    }

    Date &operator += (int d) {
        day_count += d;
        std::chrono::sys_days tmp((std::chrono::days(day_count)));
        auto ymd = std::chrono::year_month_day(tmp);
        year = ymd.year().operator int();
        month = ymd.month().operator unsigned();
        day = ymd.day().operator unsigned();
        return *this;
    }


    static int count_month_day(int year, int month) {
        if (month == 2) {
            return (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0) ? 29 : 28;
        }
        constexpr int days[] = {0, 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return days[month];
    }

    static Date now() {
        return std::chrono::system_clock::now();
    }
};


