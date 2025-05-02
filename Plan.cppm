module;
#include <string>
#include <iostream>
#include <chrono>
#include <QJsonObject>
#include <QJsonDocument>
export module Plan;

import Time;


bool CompareDate(nl::Time time1, nl::Time time2) {
    return time1.get_year() == time2.get_year()
        && time1.get_month() == time2.get_month()
        && time1.get_day() == time2.get_day();
}
bool CompareTime(nl::Time time1, nl::Time time2) {
    return time1.get_hour() == time2.get_hour()
        && time1.get_minute() == time2.get_minute()
        && time1.get_second() == time2.get_second();
}



// 对于一个计划序列化文件
// 计划类型
// 计划名称
// 计划参数
// 开始时间
// 该计划是否已经失效
export extern "C++" struct Plan {
    enum class PlanType {
        OneTimePlan, IntervalDaysPlan, FixedDatePlan, DurationPlan,
    } plan_type{};
    std::string plan_name{};
    bool need_delete{};


    size_t value{};
    enum class FixedType {
        Day, Week, Month, Year
    } fixed_type{};


    nl::Time begin_date, reminder_time;

    bool operator == (const Plan &other) const {
        return plan_name == other.plan_name 
            && plan_type == other.plan_type;
    }

    bool need_reminder(const nl::Time &time) {
        return CompareTime(reminder_time, time);
    }

    QJsonObject to_json() {
        QJsonObject json;
        json["plan_type"] = static_cast<int>(plan_type);
        json["plan_name"] = QString::fromStdString(plan_name);
        json["need_delete"] = need_delete;
        json["value"] = static_cast<int>(value);
        json["fixed_type"] = static_cast<int>(fixed_type);
        json["begin_date"] = QString::fromStdString(begin_date.to_date_string());
        json["reminder_time"] = QString::fromStdString(reminder_time.to_string());

        return json;
    }

    virtual bool active(const nl::Time &) = 0;
    virtual ~Plan() = default;
};


// 一次性的计划
// 一次性的计划完成之后应该被忽略或者被删除
export class OneTimePlan : public Plan {

public:
    OneTimePlan() {
        plan_type = PlanType::OneTimePlan;
    }

    bool active(const nl::Time & date) {
        if (CompareDate(date, begin_date))
            return true;
        need_delete = true;
        return false;
    }


};


// 固定天数循环一次的计划
export class IntervalDaysPlan : public Plan {
public:
    IntervalDaysPlan(size_t interval) {
        plan_type = PlanType::IntervalDaysPlan;
        value = interval;
    }

    bool active(const nl::Time & date) {
        auto day = (date - begin_date).count<std::chrono::milliseconds>();
        if (CompareDate(date, begin_date))
            return true;

        // 没有到达触发时间
        if (day % value)
            return false;
        return true;

    }

};


// 特定日期的触发任务
export class FixedDatePlan : public Plan {


public:

    FixedDatePlan(FixedType type, size_t val) : Plan() {
        plan_type = PlanType::FixedDatePlan;
        fixed_type = type;
        value = val;
    }

    bool active(const nl::Time & date) {
        bool flag{};
        switch (fixed_type) {
            case FixedType::Year:
                flag = value == date.get_year();
                break;
            case FixedType::Month:
                flag = value == date.get_month();
                break;
            case FixedType::Day:
                flag = value == date.get_day();
                break;
            case FixedType::Week:
                flag = value == date.get_week();
                break;
        }
        return flag;
    }

};


// 持续特定时间的计划
export class DurationPlan : public Plan {

public:
    DurationPlan(size_t duration) {
        plan_type = PlanType::DurationPlan;
        value = duration;
    }

    bool active(const nl::Time & date) {
        auto duration_day = (date - begin_date).count<std::chrono::days>();
        if (duration_day < value)
            return true;
        need_delete = true;
        return false;
    }


};

