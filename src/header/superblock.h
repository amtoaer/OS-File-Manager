//
// Created by amtoaer on 2021/5/27.
//

#ifndef OS_FILE_MANAGER_SUPERBLOCK_H
#define OS_FILE_MANAGER_SUPERBLOCK_H

#include <bits/stdc++.h>
#include "config.h"

using namespace std;

class SuperBlock {         //超级块结构
private:
    int freeinode_num;      //空闲i结点总数
    vector<int> freeInode; //空闲i结点id数组

    int freedisk_num;        //空闲磁盘块块数
    int freeDiskStack[NICFREE + 1]; //成组链接空闲磁盘块

    int freedir_num;
    bool sfdBitmap[SFDNUM];      //目录位示图   false:空  true:满
public:
    //格式化
    void format();

    //获取空闲i结点
    int getFreeInode();

    //获取空闲目录
    int getFreeDir();

    //获取空闲i结点数
    int getFreeInodeNum() {
        return freeinode_num;
    }

    //获取空闲目录数
    int getFreeDirNum() {
        return freedir_num;
    }

    //获取空闲磁盘数
    int getFreeDiskNum() {
        return freedisk_num;
    }
};

#endif //OS_FILE_MANAGER_SUPERBLOCK_H
