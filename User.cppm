/********************************************************************************
* @author : hexne
* @date   : 2025/04/18 14:50:12
********************************************************************************/

module;
#include <vector>
#include <memory>
export module User;

import Plan;
import Time;

export class User {
	bool is_local_;
	size_t id{};
	nl::Time reminder_time_;
	
public:
	std::vector<std::shared_ptr<Plan>> plans, done_plans, delete_plans;
	bool is_local() const {
		return is_local_;
	}

	// 只获取今天有效的计划
	std::vector<std::shared_ptr<Plan>> get_cur_date_plans() {
		std::vector<std::shared_ptr<Plan>> ret;
		auto cur_time = nl::Time::now();

		for (auto plan : plans) {
			if (plan->active(cur_time))
				ret.push_back(plan);
		}
		return ret;
	}

};

