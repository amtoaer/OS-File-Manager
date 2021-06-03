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
    bool inodeBitmap[DINODENUM]; //i结点位示图

    int freedisk_num;        //空闲磁盘块块数
    int freeDiskStack[NICFREE + 1]; //成组链接空闲磁盘块
    bool diskBitmap[DISKNUM];   //磁盘块位示图

    int freedir_num;
    bool sfdBitmap[SFDNUM];      //目录位示图   false:空  true:满

    // 清空空闲磁盘栈
    void clearFreeDiskStack(int);

    // 将磁盘块调入空闲磁盘块栈
    void copy(DiskBlock);

    //将栈内容写入磁盘块
    void writeStack(int loc);

    // 添加到空闲磁盘块栈
    void pushFreeDiskStack(int);

    // 移出空闲磁盘块栈
    int popFreeDiskStack();

    //保存i结点空闲信息
    void saveFreeInodeInfo();

    // 读取i结点空闲信息
    bool readFreeInodeInfo();

    //保存磁盘块空闲信息
    void saveFreeDiskInfo();

    // 读取磁盘块空闲信息
    bool readFreeDiskInfo();

    //保存目录空闲信息
    void saveFreeDirInfo();

    // 读取目录空闲信息
    bool readFreeDirInfo();

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

    //获取已占i结点数
    int getUsedInodeNum();

    //获取已占目录数
    int getUsedDirNum();

    //获取已占磁盘数
    int getUsedDiskNum();

    //保存信息到文件
    void saveTofile();

    // 从文件读取
    bool readFromFile();

    //i结点是否被占用
    bool isInodeUsed(int id);

    //磁盘是否被占用
    bool isDiskUsed(int id);

    //目录是否被占用
    bool isDirUsed(int id);
};

#endif //OS_FILE_MANAGER_SUPERBLOCK_H
