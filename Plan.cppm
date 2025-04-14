module;
#include <string>
export module Plan;

import Date;

export struct Plan {
    enum class PlanType {
        OneTimePlan, IntervalDaysPlan, FixedDatePlan, DurationPlan
    } plan_type;
    std::string plan_name{};

    bool operator == (const Plan &other) const {
        return plan_name == other.plan_name && plan_type == other.plan_type;
    }

    Date start_date;

};


// 一次性的计划
// 一次性的计划完成之后应该被忽略或者被删除
export class OneTimePlan : Plan {

};


// 固定天数循环一次的计划
export class IntervalDaysPlan : Plan{
    size_t interval_days_{1};

};


// 特定日期的任务
export class FixedDatePlan : Plan{


};


// 持续特定时间的计划
export class DurationPlan : Plan {

};