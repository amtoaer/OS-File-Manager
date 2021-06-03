//
// Created by lenovo on 2021-05-29.
//

#include "../header/user.h"

vector<int> UserManage::getUserId() {
    vector<int> user_id;
    for (int i = 0; i < USERNUM; i++) {
        if (users[i].getId() != -1) {
            user_id.push_back(i);
        }
    }
    return user_id;
}

bool UserManage::addUser(string username, string userpwd) {//注册一个用户
    for (int i = 0; i < USERNUM; i++) {
        if (users[i].getId() == -1) {
            users[i].setId(i);
            users[i].setUsername(username);
            users[i].setUserpwd(userpwd);
            user_num++;
            return true;//注册成功
        }
    }
    return false;//用户已满，注册失败
}

bool UserManage::deleteUser(string username, string userpwd) {//注销一个用户
    for (auto &user : users) {
        if (user.isUsername(username)) {
            if (!user.isUserpwd(userpwd)) {
                return false;//密码错误
            }
            user.setId(-1);
            user.setUsername("");
            user.setUserpwd("");
            user_num--;
            return true;//注销成功
        }
    }
    return false;//未在用户信息中查到此人
}

bool UserManage::login(string username, string userpwd) {//用户登陆
    for (User user : users) {
        if (user.isUsername(username)) {
            if (user.isUserpwd(userpwd)) {
                view.cur_user = user;
                return true;//登陆成功，用户为user
            }
            return false;//密码错误
        }
    }
    return false;//未在用户信息中查到此人
}

int UserManage::getId(string name) { //通过用户名得到用户id
    for (User user: users) {
        if (user.isUsername(name)) {
            return user.getId();
        }
    }
    return -1;
}