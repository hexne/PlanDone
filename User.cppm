
module;
#include <memory>
#include <vector>
export module User;

import Date;
import Plan;

export class User {
    bool is_local_ = true;
    size_t id{};
public:

    std::vector<std::shared_ptr<Plan>> plans, expired_plan;

    bool is_local() const {
        return is_local_;
    }

    // 只获取今天有效的计划
    std::vector<std::shared_ptr<Plan>> get_cur_date_plans() {
        std::vector<std::shared_ptr<Plan>> ret;
        auto cur_date = Date::now();

        for (auto plan : plans) {
            if (plan->active(cur_date))
                ret.push_back(plan);
        }
        return ret;
    }


};


