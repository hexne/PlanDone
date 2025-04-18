/********************************************************************************
* @author : hexne
* @date   : 2025/04/18 14:50:12
********************************************************************************/

#include "User.h"

bool User::is_local() const {
    return is_local_;
}

// 只获取今天有效的计划
std::vector<std::shared_ptr<Plan>> User::get_cur_date_plans() {
    std::vector<std::shared_ptr<Plan>> ret;
    auto cur_date = Date::now();

    for (auto plan : plans) {
        if (plan->active(cur_date))
            ret.push_back(plan);
    }
    return ret;
}




