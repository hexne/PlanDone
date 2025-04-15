
module;
#include <memory>
#include <vector>
export module User;

import Calendar;
import Date;
import Plan;

class User {
    bool is_local_ = true;
    size_t id{};
    Calendar calendar_;
    // 这里只保留用户的计划？
    // 在显示的时候，或者当计算当前日期有哪些计划的时候，从用户计划中判断这些计划哪些有效
    std::vector<std::shared_ptr<Plan>> plans_, expired_plan_;
public:
    bool is_local() const {
        return is_local_;
    }

    // 只获取今天有效的计划
    std::vector<std::shared_ptr<Plan>> &get_cur_date_plans() {
        std::vector<std::shared_ptr<Plan>> ret;
        auto cur_date = Date::now();

        for (auto plan : plans_) {
            if (plan->active(cur_date))
                ret.push_back(plan);
        }
        return ret;
    }


};


