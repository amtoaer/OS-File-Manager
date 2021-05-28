//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_DIR_H
#define OS_FILE_MANAGER_DIR_H

#include <bits/stdc++.h>

struct SFD_ITEM {
    int type;       //目录 或 文件
    string name;    //命名
    int id;         //目录：下一级目录索引  文件：磁盘i结点
};

struct MFD_ITEM {
    int userid;   //用户名
    int dir_id; //目录id
};

struct SFD {
    int num;
    vector <SFD_ITEM> dir;

    SFD() {
        num = 0;
    }
};

struct BFD {
    int ffd_num;        //空闲目录数
    vector<int> FFD;    //空闲目录id

    int mfd_num;          //用户总数
    vector <MFD_ITEM> MFD; //用户目录id

};

#endif //OS_FILE_MANAGER_DIR_H
