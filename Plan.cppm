module;
#include <string>
export module Plan;

import Date;


// 对于一个计划序列化文件
// 计划类型
// 计划名称
// 计划参数
// 开始时间
export struct Plan {
    enum class PlanType {
        OneTimePlan, IntervalDaysPlan, FixedDatePlan, DurationPlan
    } plan_type;
    std::string plan_name{};
    bool need_delete{};

    bool operator == (const Plan &other) const {
        return plan_name == other.plan_name && plan_type == other.plan_type;
    }

    Date start_date;

    virtual bool active(const Date &) = 0;
    virtual ~Plan() = default;
};


// 一次性的计划
// 一次性的计划完成之后应该被忽略或者被删除
export class OneTimePlan : public Plan {

public:
    OneTimePlan() {
        plan_type = PlanType::OneTimePlan;
    }

    bool active(const Date & date) {
        if (date == start_date)
            return true;
        need_delete = true;
        return false;
    }


};


// 固定天数循环一次的计划
export class IntervalDaysPlan : Plan {
    size_t interval_days_{};
public:
    IntervalDaysPlan(size_t interval) : interval_days_(interval) {
        plan_type = PlanType::IntervalDaysPlan;
    }

    bool active(const Date & date) {
        auto day = date - start_date;
        if (date == start_date)
            return true;

        // 没有到达触发时间
        if (day % interval_days_)
            return false;
        return true;

    }

};


// 特定日期的触发任务
export class FixedDatePlan : Plan {
    size_t year_{}, month_{}, day_{}, week_{};
    enum class FixedType {
        Year, Month, Day, Week
    } fixed_type_;

public:

    FixedDatePlan(FixedType fixed_type, size_t val) : Plan() {
        plan_type = PlanType::FixedDatePlan;
        switch (fixed_type) {
            case FixedType::Year:
                year_ = val;
                break;
            case FixedType::Month:
                month_ = val;
                break;
            case FixedType::Day:
                day_ = val;
                break;
            case FixedType::Week:
                week_ = val;
                break;
        }
    }

    bool active(const Date & date) {
        bool flag{};
        switch (fixed_type_) {
            case FixedType::Year:
                flag = start_date.year == date.year;
                break;
            case FixedType::Month:
                flag = start_date.month == date.month;
                break;
            case FixedType::Day:
                flag = start_date.day == date.day;
                break;
            case FixedType::Week:
                flag = start_date.week == date.week;
                break;
        }
        return flag;
    }

};


// 持续特定时间的计划
export class DurationPlan : Plan {
    size_t duration_{};

public:
    DurationPlan(size_t duration) : duration_(duration) {
        plan_type = PlanType::DurationPlan;
    }

    bool active(const Date & date) {
        auto duration_day = date - start_date;
        if (duration_day < duration_)
            return true;
        need_delete = true;
        return false;
    }


};