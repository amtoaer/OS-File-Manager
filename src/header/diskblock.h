//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_DISKBLOCK_H
#define OS_FILE_MANAGER_DISKBLOCK_H

#include "config.h"

struct Doc {
    char content[BLOCKSIZE];    //文件内容
    int n;        //已占空间
};

struct Index {
    int p[BLOCKSIZE / sizeof(int)]; //索引集合
    int n;  //索引总数
};

union DiskBlock { //磁盘块
    Doc doc;
    Index index;
};

#endif //OS_FILE_MANAGER_DISKBLOCK_H
