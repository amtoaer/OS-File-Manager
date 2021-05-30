//
// Created by lenovo on 2021-05-28.
//

#include "../header/superblock.h"

void SuperBlock::format() {
    //初始化空闲i结点信息
    freeinode_num = DINODENUM;
    for (int i = 0; i < DINODENUM; i++) {
        freeInode.push_back(i);
    }

    //初始化空闲磁盘块信息
    freedisk_num = DISKNUM;
    //成组链接初始化 空闲块id为0-DISKNUM-1###########

    //初始化目录块信息
    freedir_num = DIRNUM - 1;
    memset(sfdBitmap, false, sizeof(sfdBitmap));
    sfdBitmap[0] = true;    //0号单元作为根目录

}


int SuperBlock::getFreeInode() {
    if (freeinode_num > 0) {
        int id = freeInode[--freeinode_num];
        freeInode.pop_back();
        return id;
    }
    return -1;
}

int SuperBlock::getFreeDir() {
    if (freedir_num <= 0) return -1;
    int i = 0;
    for (; i < SFDNUM; i++) {
        if (!sfdBitmap[i]) {
            sfdBitmap[i] = true;
            freedir_num--;
            break;
        }
    }
    return i;
}
