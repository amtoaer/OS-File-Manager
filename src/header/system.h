//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_SYSTEM_H
#define OS_FILE_MANAGER_SYSTEM_H

#include <bits/stdc++.h>

#include "config.h"
#include "superblock.h"
#include "inode.h"
#include "diskblock.h"
#include "dir.h"

using namespace std;

class FileSystem {             //文件系统结构
private:
    SuperBlock superBlock;   //超级块
    Dinode diNode[DINODENUM];        //i结点
    DiskBlock diskBlock[DISKNUM];//磁盘块
    SFD sfd[SFDNUM];                   //目录块
    bool diskBitmap[DISKNUM];    //磁盘块结点位示图

public:
    // 默认初始化
    FileSystem();

    // 通过输入流初始化
    FileSystem(istream input);

    // 创建文件夹
    void mkdir(string dir);

    // 创建文件
    void touch(string filePath);

    // 写入文件
    void writeFile(string filePath, string content);

    // 删除文件
    void rm(string filePath, bool isRecursive);

    // 复制文件
    void cp(string from, string to);

    // 移动文件
    void mv(string from, string to);
};

#endif //OS_FILE_MANAGER_SYSTEM_H
