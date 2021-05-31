//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_DISKBLOCK_H
#define OS_FILE_MANAGER_DISKBLOCK_H

#include <bits/stdc++.h>
#include "config.h"

using namespace std;

struct Doc {
    char content[BLOCKSIZE];    //文件内容
    int n;        //已占空间
    Doc() {
        n = 0;
    }
};

struct Index {
    int p[BLOCKSIZE / sizeof(int)]; //索引集合
    int n;  //索引总数
    Index() {
        n = 0;
    }
};

union FileContent { //磁盘块
    Doc doc;
    Index index;

    FileContent() {}
};

class DiskBlock {
    friend class SuperBlock;

    FileContent file_cont;
public:
    DiskBlock() {};

    //写文本
    void writeText(string text);

    //获取文本
    void getText(char *str);

    //添加索引
    bool addIndex(int index);

    // 拷贝磁盘块
    void copy(int block[]);

    //获取磁盘inode索引 以0开始 失败返回-1
    int getDinodeId(int nth);

    //清空索引
    void clearIndex();
};

#endif //OS_FILE_MANAGER_DISKBLOCK_H
