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
import Calendar;

export extern "C++" class User {
	bool is_local_ = true;
	size_t id{};
	nl::Time reminder_time_;

public:
	std::vector<std::shared_ptr<Plan>> current_plans, done_plans, delete_plans;
    Calendar calendar;

	bool is_local() const {
		return is_local_;
	}

	// 只获取今天有效的计划
	std::vector<std::shared_ptr<Plan>> get_cur_date_plans() {
		std::vector<std::shared_ptr<Plan>> ret;
		auto cur_time = nl::Time::now();

		for (auto plan : current_plans) {
			if (plan->active(cur_time))
				ret.push_back(plan);
		}
		return ret;
	}
	auto get_calendar_iterator() {
		return calendar_.to_iterator();
	}

};

