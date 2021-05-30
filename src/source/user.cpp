//
// Created by lenovo on 2021-05-29.
//

#include "../header/user.h"

vector<int> UserManage::getUserId() {
    vector<int> user_id;
    for (int i = 0; i < user_num; i++) {
        user_id.push_back(i);
    }
    return user_id;
}