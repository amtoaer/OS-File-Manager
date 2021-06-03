//
// Created by lenovo on 2021-05-28.
//

#ifndef OS_FILE_MANAGER_VIEW_H
#define OS_FILE_MANAGER_VIEW_H

#include "dir.h"
#include "user.h"

struct View {
    User cur_user;  //当前用户
    int cur_sfd_id;    //当前目录
    string cur_path;//当前路径 /a/b/c
};

extern View view;
#endif //OS_FILE_MANAGER_VIEW_H
