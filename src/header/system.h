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

using namespace std;

class FileSystem {             //文件系统结构
private:
    SuperBlock superBlock;          //超级块
    Dinode diNode[DINODENUM];       //i结点
    DiskBlock diskBlock[DISKNUM];   //磁盘块
    SFD sfd[SFDNUM];                //目录块
    int root_id = 0;    //根目录默认占用

    string getFullPath(string);

    //获取文件所占磁盘块id集合
    vector<int> getFileDiskIds(int inode_id);

    //获取文件内容所占磁盘块id集合
    vector<int> getFileContentDiskIds(int inode_id);

    //计算给定长度文件需要占用的磁盘块
    int calculateDiskNum(int len);

    //将文件内容写到磁盘块
    void writeToDiskBlock(vector<int> applied_disk, int inode_i, int start_block, string content);

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

    // 写入文件（覆盖）
    bool writeFile(string filePath, string content);

    //追加到文件
    bool appendToFile(string filePath, string content);

    //读文件
    string readFile(string filePath);

    // 删除文件
    bool rm(string filePath);

    // 复制文件
    bool cp(string from, string to);

    // 移动文件
    bool mv(string from, string to);

    //查找目录 返回目录id
    int findDir(vector<string>);

    // 查找目录字符串 返回目录id
    int findDir(string);

    //查找文件 返回i结点id
    int findFile(string path);

    // 返回对应位置的磁盘块
    DiskBlock getDiskBlock(int index);

};

extern FileSystem fs;

#endif //OS_FILE_MANAGER_SYSTEM_H
