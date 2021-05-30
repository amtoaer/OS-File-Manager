//
// Created by lenovo on 2021-05-28.
//

#include "../header/superblock.h"

void SuperBlock::clearFreeDiskStack(int next = -1) {
    freeDiskStack[0] = 1;
    freeDiskStack[1] = next;
}

void SuperBlock::copy(DiskBlock db) {
    freeDiskStack[0] = db.file_cont.index.n;
    for (int i = 1; i <= NICFREE; i++) {
        freeDiskStack[i] = db.file_cont.index.p[i - 1];
    }
}

void SuperBlock::pushFreeDiskStack(int loc) {
    freeDiskStack[0]++;
    freeDiskStack[freeDiskStack[0]] = loc;
}

int SuperBlock::popFreeDiskStack() {
    int result = freeDiskStack[freeDiskStack[0]];
    freeDiskStack[0]--;
    return result;
}

void SuperBlock::format() {
    //初始化空闲i结点信息
    freeinode_num = DINODENUM;
    for (int i = 0; i < DINODENUM; i++) {
        freeInode.push_back(i);
    }

    //初始化空闲磁盘块信息
    freedisk_num = DISKNUM;

    //成组链接初始化 空闲块id为0-DISKNUM-1###########

    // 初始化空闲磁盘块栈
    clearFreeDiskStack();
    // 遍历回收
    for (int i = 0; i < DISKNUM; i++) {
        recycleDiskBlock(i);
    }

    //初始化目录块信息
    freedir_num = DIRNUM - 1;
    memset(sfdBitmap, false, sizeof(sfdBitmap));
    sfdBitmap[0] = true;    //0号单元作为根目录

}

void SuperBlock::recycleDiskBlock(int loc) {
    if (freeDiskStack[0] == NICFREE) {
        // 如果已满则拷贝空闲磁盘块栈
        fs.getDiskBlock(loc).copy(freeDiskStack);
        // 清空当前空闲磁盘块栈
        clearFreeDiskStack(loc);
    }
    // 添加到磁盘块栈
    pushFreeDiskStack(loc);
}

int SuperBlock::getFreeDiskBlock() {
    if (freeDiskStack[0] == 1) {
        if (freeDiskStack[1] == -1) {
            // 空闲磁盘块为空
            return -1;
        }
        // 调入磁盘栈
        this->copy(fs.getDiskBlock(freeDiskStack[1]));
    }
    return popFreeDiskStack();
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
