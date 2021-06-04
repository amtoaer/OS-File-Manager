//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_USER_H
#define OS_FILE_MANAGER_USER_H

#include "config.h"
#include <bits/stdc++.h>

using namespace std;


class User {      //用户
    int id = -1;         //用户id号
    string username; //用户名
    string userpwd;   //密码
public:
    User() {};

    User(int id) {
        this->id = id;
    }

    User(int id, string name, string pwd) {
        this->id = id;
        this->username = name;
        this->userpwd = pwd;
    }

    //获取用户id
    int getId() {
        return id;
    }

    string getUsername() {
        return username;
    }

    string getPassword() {
        return userpwd;
    }

    void setId(int id) {
        this->id = id;
    }

    void setUsername(string username) {
        this->username = username;
    }

    bool isUsername(string username) {
        return this->username == username;
    }

    void setUserpwd(string userpwd) {
        this->userpwd = userpwd;
    }

    bool isUserpwd(string userpwd) {
        return this->userpwd == userpwd;
    }
};

class UserManage {
    User users[USERNUM];  //全部用户
    int user_num = 0;   //当前已有用户
    void readFromFile(); //从文件读取

public:
    UserManage();

    bool addUser(string username, string userpwd);     //添加用户
    bool deleteUser(string username, string userpwd);  //注销用户
    bool login(string username, string userpwd);       //登陆用户
    vector<int> getUserId();    //获取全部用户的id号
    vector<string> getUserName();
    string getName(int);
    int getId(string name);     //通过用户名得到id
    void saveToFile();
};

extern UserManage user_mag;

#endif //OS_FILE_MANAGER_USER_H
