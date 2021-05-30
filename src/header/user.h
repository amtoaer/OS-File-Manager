//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_USER_H
#define OS_FILE_MANAGER_USER_H

#include "config.h"
#include <bits/stdc++.h>

using namespace std;


class User {      //用户
    int id;         //用户id号
    string username; //用户名
    string userpwd;   //密码
public:
    User() {};

    User(int id, string name, string pwd);

    //获取用户id
    int getId() {
        return id;
    }
};

class UserManage {
    User users[USERNUM];  //全部用户
    int user_num = 0;   //当前已有用户
public:
    bool addUser();     //添加用户
    bool deleteUser();  //注销用户
    vector<int> getUserId();    //获取全部用户的id号
};

#endif //OS_FILE_MANAGER_USER_H
