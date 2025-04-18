/********************************************************************************
* @author : hexne
* @date   : 2025/04/18 14:50:13
********************************************************************************/

#pragma once

#include <memory>

import Date;
import Plan;

class User {
    bool is_local_ = true;
    size_t id{};
public:

    std::vector<std::shared_ptr<Plan>> plans, expired_plan;

    bool is_local() const;

    // 只获取今天有效的计划
    std::vector<std::shared_ptr<Plan>> get_cur_date_plans();


};



