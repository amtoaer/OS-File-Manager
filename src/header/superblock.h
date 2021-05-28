//
// Created by amtoaer on 2021/5/27.
//

#ifndef OS_FILE_MANAGER_SUPERBLOCK_H
#define OS_FILE_MANAGER_SUPERBLOCK_H

#include "config.h"

extern int UserN = 0;   //当前已有用户


struct SuperBlock {         //超级块结构
    int freeinode_num;      //空闲i结点总数
    vector<int> freeInode; //空闲i结点id数组

    int freedisk_num;        //空闲磁盘块块数
    int freeDiskStack[NICFREE + 1]; //成组链接空闲磁盘块栈


};

extern FileSystem FS;    //文件系统
extern bool diBitmap[DINODENUM];    //磁盘i结点位示图
extern bool diskBitmap[DISKNUM];    //磁盘块结点位示图
extern bool sfdBitmap[SFDNUM];      //目录位示图
extern int sfd_id;      //当前目录id
extern User Users[USERNUM];  //全部用户
extern User cur_user;  //当前用户

#endif //OS_FILE_MANAGER_SUPERBLOCK_H
