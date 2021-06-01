//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_DIR_H
#define OS_FILE_MANAGER_DIR_H

#include <bits/stdc++.h>
#include "config.h"

using namespace std;

struct SFD_ITEM {
    int type;       //FILETYPE:1 DIRTYPE:2
    string name;    //命名
    int id;         //目录：下一级目录索引  文件：磁盘i结点索引
    SFD_ITEM() {}

    SFD_ITEM(int t, string nam, int i) {
        type = t;
        name = nam;
        id = i;
    }
};

class SFD {
private:
    int num;    //当前目录下文件或目录总量
    SFD_ITEM dir[DIRNUM];
public:
    SFD() {
        num = 0;
    }

    //能否添加文件或目录
    bool canAddItem() {
        return num < DIRNUM;
    }

    //添加文件或目录
    bool addItem(SFD_ITEM sfd_item);

    //获取下一级目录id 失败返回-1
    int getNextDir(string dirname);

    // 返回指定名称的文件/文件夹（不存在返回0,存在文件返回1,存在目录返回2）
    int hasNext(string name);

    // 删除指定名称的文件/文件夹
    SFD_ITEM removeNext(string name);

    // 获取全部文件/文件夹
    vector<SFD_ITEM> getAllNext();

    //获取目录下文件i结点id 失败返回-1
    int getFileInode(string filename);

    int getNum() {  //获取已存放量
        return num;
    }

};

#endif //OS_FILE_MANAGER_DIR_H
