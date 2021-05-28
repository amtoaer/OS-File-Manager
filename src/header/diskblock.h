//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_DISKBLOCK_H
#define OS_FILE_MANAGER_DISKBLOCK_H

struct DiskBlock {      //磁盘块
    char content[BLOCKSIZE];    //文件内容
    int n;        //已占空间
};

#endif //OS_FILE_MANAGER_DISKBLOCK_H
