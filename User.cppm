
module;
#include <memory>
#include <vector>
export module User;

import Date;

class User {
    bool is_local_ = true;
    size_t id_;
    Calendar calendar_;
    std::vector<std::shared_ptr<User>> plans_;
public:
    bool is_local() const {
        return is_local_;
    }



};


