//
// Created by amtoaer on 2021/5/27.
//

#ifndef OS_FILE_MANAGER_SUPERBLOCK_H
#define OS_FILE_MANAGER_SUPERBLOCK_H

#include <bits/stdc++.h>
#include "config.h"
#include "diskblock.h"

using namespace std;

class SuperBlock {         //超级块结构
private:
    int freeinode_num;      //空闲i结点总数
    vector<int> freeInode; //空闲i结点id数组

    int freedisk_num;        //空闲磁盘块块数
    int freeDiskStack[NICFREE + 1]; //成组链接空闲磁盘块

    int freedir_num;
    bool sfdBitmap[SFDNUM];      //目录位示图   false:空  true:满

    // 清空空闲磁盘栈
    void clearFreeDiskStack(int);

    // 将磁盘块调入空闲磁盘块栈
    void copy(DiskBlock);

    // 添加到空闲磁盘块栈
    void pushFreeDiskStack(int);

    // 移出空闲磁盘块栈
    int popFreeDiskStack();

public:
    //格式化
    void format();

    // 回收磁盘块
    void recycleDiskBlock(int);

    // 获取空闲磁盘块
    int getFreeDiskBlock();

    //获取空闲i结点
    int getFreeInode();

    // 回收空闲i节点
    void recycleInode(int);

    //获取空闲目录
    int getFreeDir();

    // 回收空闲目录
    void recycleDir(int);

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

extern SuperBlock sb;

#endif //OS_FILE_MANAGER_SUPERBLOCK_H
