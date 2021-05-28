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


class FileSystem {             //文件系统结构

    SuperBlock superBlock;   //超级块
    Dinode diNode[DINODENUM];        //i结点
    DiskBlock diskBlock[DISKNUM];//磁盘块
    SFD sfd[SFDNUM];                   //目录块

    FileSystem();
};

#endif //OS_FILE_MANAGER_SYSTEM_H
