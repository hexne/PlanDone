module;
#include <string>
#include <chrono>
export module Plan;

import Time;

// 对于一个计划序列化文件
// 计划类型
// 计划名称
// 计划参数
// 开始时间
// 该计划是否已经失效
export struct Plan {
    enum class PlanType {
        OneTimePlan, IntervalDaysPlan, FixedDatePlan, DurationPlan,
    } plan_type{};
    std::string plan_name{};
    bool need_delete{};


    size_t value{};
    enum class FixedType {
        Year, Month, Day, Week
    } fixed_type{};


    nl::Time start_date;

    bool operator == (const Plan &other) const {
        return plan_name == other.plan_name && plan_type == other.plan_type;
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
        if (date == start_date)
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
        auto day = (date - start_date).count<std::chrono::milliseconds>();
        if (date == start_date)
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
        auto duration_day = (date - start_date).count<std::chrono::days>();
        if (duration_day < value)
            return true;
        need_delete = true;
        return false;
    }


};