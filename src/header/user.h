//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_USER_H
#define OS_FILE_MANAGER_USER_H

class User {      //用户
    int id;     //用户id号
    string username;
    string userpwd;
    int usergrp;//用户组
    bool isManager; //管理员
    User();

    User(string name, string pwd, int grp, bool isM);
};

#endif //OS_FILE_MANAGER_USER_H
