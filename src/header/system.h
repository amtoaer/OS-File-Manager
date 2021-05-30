//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_SYSTEM_H
#define OS_FILE_MANAGER_SYSTEM_H

#include "config.h"
#include "superblock.h"
#include "inode.h"
#include "diskblock.h"
#include "dir.h"
#include <bits/stdc++.h>
#include<windows.h>

using namespace std;

class FileSystem {             //文件系统结构
private:
    SuperBlock superBlock;          //超级块
    Dinode diNode[DINODENUM];       //i结点
    DiskBlock diskBlock[DISKNUM];   //磁盘块
    SFD sfd[SFDNUM];                //目录块
    int root_id = 0;    //根目录默认占用

    //分割字符串
    vector<string> split(string str, string pattern);

public:
    // 默认初始化
    FileSystem();

    // 通过输入流初始化
    FileSystem(istream input);

    //格式化
    void format();

    // 创建文件夹
    bool mkdir(string dir);

    // 创建文件
    bool touch(string filePath);

    // 写入文件
    bool writeFile(string filePath, string content);

    // 删除文件
    bool rm(string filePath, bool isRecursive);

    // 复制文件
    bool cp(string from, string to);

    // 移动文件
    bool mv(string from, string to);

    //查找目录 返回目录id
    int findDir(string path);

    //查找文件 返回i结点id
    int findFile(string path);

};

#endif //OS_FILE_MANAGER_SYSTEM_H
