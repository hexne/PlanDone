/********************************************************************************
* @author : hexne
* @Time   : 2025/04/14 22:41:01
********************************************************************************/

module;
#include <vector>
#include <ranges>
#include <chrono>
#include <memory>
#include <QJsonObject>
#include <QJsonArray>
export module Calendar;

import Time;
import Plan;

export extern "C++" class Calendar {
    // using Date = std::pair<nl::Time, std::vector<std::shared_ptr<Plan>>>;
    // @TODO, 为了快速实现，此处仅在日历中保存计划名称
    using Date = std::pair<nl::Time, std::vector<std::string>>;
    using Month = std::vector<Date>;
    using Year = std::vector<Month>;
    std::vector<Year> calendar_{};
public:
    Calendar() {
        nl::Time end_date;
        if (end_date < nl::Time::FromDate(2025, 1, 1))
            return;

        for (int year = 2025; year <= end_date.get_year(); year++) {
            Year push_year;
            for (int month = 1; month <= 12; month++) {
                Month push_month;
                if (year == end_date.get_year() && month > end_date.get_month())
                    break;
                for (int day = 1; day <= nl::Time::count_month_day(year, month); day++) {
                    if (year == end_date.get_year() && month == end_date.get_month() && day > end_date.get_day())
                        break;
                    push_month.push_back({ nl::Time::FromDate(year, month, day), {} });
                }
                push_year.push_back(push_month);
            }
            calendar_.push_back(push_year);
        }
    }

    bool find_cur_done_plan(std::string plan_name) {
        auto [date, plans] = operator [](nl::Time::now());

        const auto it = std::ranges::find_if(plans,[plan_name](auto cur_plan_name) {
            return cur_plan_name == plan_name;
        });
        return it != plans.end();
    }

    auto to_iterator() {
        return calendar_ | std::ranges::views::join | std::ranges::views::join;
    }

    void operator[] (const nl::Time& date, const std::vector<std::string>& plans) {
        operator[](date).second = plans;
    }

    QJsonObject to_json() {
        QJsonObject json;
        for (const auto &[date, plans]: to_iterator()) {
            QJsonArray plans_json;
            for (const auto plan : plans) {
                plans_json.append(plan.data());
            }
            json[date.to_date_string().data()] = plans_json;
        }
        return json;
    }

    Date& operator[](const nl::Time& date) {
        return operator[](date.get_year(), date.get_month(), date.get_day());
    }
	Date& operator[](size_t year, size_t month, size_t day) {
		return calendar_[year - 2025][month-1][day-1];
	}

};

export Calendar CreateCalendar(QJsonObject &json) {
    Calendar calendar;
    for (auto it = json.begin(); it != json.end(); ++it) {
        auto date = it.key();
        auto plans_json = it.value().toArray();
        nl::Time cur_time = nl::Time::FromDate(date.toStdString());
        std::vector<std::string> plans;

        for (const auto &plan_json : plans_json)
            plans.push_back(plan_json.toString().toStdString());

        calendar[cur_time, plans];
    }
    return calendar;
}