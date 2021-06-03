//
// Created by lenovo on 2021-05-28.
//

#include "../header/superblock.h"
#include "../header/system.h"

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

void SuperBlock::writeStack(int loc) {
    fs.diskBlock[loc].file_cont.index.n = freeDiskStack[0];
    for (int i = 1; i <= NICFREE; i++) {
        fs.diskBlock[loc].file_cont.index.p[i - 1] = freeDiskStack[i];
    }
}

void SuperBlock::pushFreeDiskStack(int loc) {
    freedisk_num++;
    freeDiskStack[0]++;
    freeDiskStack[freeDiskStack[0]] = loc;
}

int SuperBlock::popFreeDiskStack() {
    freedisk_num--;
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
    memset(inodeBitmap, false, sizeof(inodeBitmap));

    //初始化空闲磁盘块信息
    freedisk_num = 0;
    // 初始化空闲磁盘块栈
    clearFreeDiskStack();
    // 遍历回收
    for (int i = 0; i < DISKNUM; i++) {
        recycleDiskBlock(i);
    }
    memset(diskBitmap, false, sizeof(diskBitmap));

    //初始化目录块信息
    freedir_num = DIRNUM - 1;
    memset(sfdBitmap, false, sizeof(sfdBitmap));
    sfdBitmap[0] = true;    //0号单元作为根目录

}

void SuperBlock::recycleDiskBlock(int loc) {
    if (freeDiskStack[0] == NICFREE) {
        // 如果已满则拷贝空闲磁盘块栈
        //fs.getDiskBlock(loc).copy(freeDiskStack);
        writeStack(loc);
        // 清空当前空闲磁盘块栈
        clearFreeDiskStack(loc);
        freedisk_num++;
    } else {
        // 添加到磁盘块栈
        pushFreeDiskStack(loc);
    }
    diskBitmap[loc] = false;
}

int SuperBlock::getFreeDiskBlock() {
    if (freeDiskStack[0] == 1) {
        if (freeDiskStack[1] == -1) {
            // 空闲磁盘块为空
            return -1;
        }
        int free = freeDiskStack[1];
        // 调入磁盘栈
        this->copy(fs.getDiskBlock(freeDiskStack[1]));
        freedisk_num--;
        diskBitmap[free] = true;
        return free;
    } else {
        int free = popFreeDiskStack();
        diskBitmap[free] = true;
        return free;
    }

}

int SuperBlock::getFreeInode() {
    if (freeinode_num > 0) {
        int id = freeInode[--freeinode_num];
        freeInode.pop_back();
        inodeBitmap[id] = true;
        return id;
    }
    return -1;
}

void SuperBlock::recycleInode(int inode) {
    inodeBitmap[inode] = false;
    freeinode_num++;
    freeInode.push_back(inode);
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

void SuperBlock::recycleDir(int dir) {
    freedir_num++;
    sfdBitmap[dir] = false;
}

void SuperBlock::saveFreeInodeInfo() {
    ofstream outfile;
    //存空闲结点信息
    outfile.open("../records/freeIdode.txt", ios::out | ios::trunc);
    if (!outfile.is_open()) {
        cout << "文件打开失败readFreeDir!" << endl;
        return;
    }
    outfile << freeinode_num << " ";
    for (int i = 0; i < freeInode.size(); i++) {
        outfile << freeInode[i] << " ";
    }
    //存位示图信息
    outfile << DINODENUM << " ";
    for (int i = 0; i < DINODENUM; i++) {
        outfile << inodeBitmap[i] << " ";
    }
    outfile.close();
}

bool SuperBlock::readFreeInodeInfo() {
    ifstream input;
    input.open("../records/freeInode.txt", ios::in);
    if (!input.is_open()) {
        return false;
    }
    input >> freeinode_num;
    int tmp;
    for (int i = 0; i < DINODENUM; i++) {
        input >> tmp;
        inodeBitmap[i] = tmp;
    }
    input.close();
    return true;
}

void SuperBlock::saveFreeDiskInfo() {
    ofstream outfile;
    //存空闲块信息
    outfile.open("../records/freeDisk.txt", ios::out | ios::trunc);
    if (!outfile.is_open()) {
        cout << "文件打开失败!" << endl;
        return;
    }
    outfile << freedisk_num << " ";

    int id = getFreeDiskBlock();
    while (id != -1) {
        outfile << id << " ";
        diskBitmap[id] = false;
        id = getFreeDiskBlock();
    }
    for (int i = 0; i < DISKNUM; i++) {
        outfile << diskBitmap[i] << " ";
    }

    outfile.close();
}

bool SuperBlock::readFreeDiskInfo() {
    ifstream input;
    input.open("../records/freeDisk.txt", ios::in);
    if (!input.is_open()) {
        return false;
    }
    int count, tmp;
    input >> count;
    while (count--) {
        input >> tmp;
        recycleDiskBlock(tmp);
    }
    input >> count;
    for (int i = 0; i < DISKNUM; i++) {
        input >> tmp;
        diskBitmap[i] = tmp;
    }
    input.close();
    return true;
}

void SuperBlock::saveFreeDirInfo() {
    ofstream outfile;
    //存空闲块信息
    outfile.open("../records/freeDir.txt", ios::out | ios::trunc);
    if (!outfile.is_open()) {
        cout << "文件打开失败!" << endl;
        return;
    }
    outfile << freedir_num << " ";
    for (int i = 0; i < DIRNUM; i++) {
        outfile << sfdBitmap[i] << " ";
    }
    outfile.close();
}

bool SuperBlock::readFreeDirInfo() {
    ifstream input;
    input.open("../records/freeDir.txt", ios::in);
    if (!input.is_open()) {
        return false;
    }
    input >> freedir_num;
    int tmp;
    for (int i = 0; i < DIRNUM; i++) {
        input >> tmp;
        sfdBitmap[i] = tmp;
    }
    input.close();
    return true;
}

int SuperBlock::getUsedInodeNum() {
    int sum = 0;
    for (int i = 0; i < DINODENUM; i++) {
        if (inodeBitmap[i])
            sum++;
    }
    return sum;
}

int SuperBlock::getUsedDirNum() {
    int sum = 0;
    for (int i = 0; i < DIRNUM; i++) {
        if (sfdBitmap[i])
            sum++;
    }
    return sum;
}

int SuperBlock::getUsedDiskNum() {
    int sum = 0;
    for (int i = 0; i < DISKNUM; i++) {
        if (diskBitmap[i])
            sum++;
    }
    return sum;
}

void SuperBlock::saveTofile() {
    saveFreeInodeInfo();
    saveFreeDiskInfo();
    saveFreeDirInfo();
}

bool SuperBlock::readFromFile() {
    return readFreeDirInfo() & readFreeDiskInfo() & readFreeInodeInfo();
}

bool SuperBlock::isInodeUsed(int id) {
    return inodeBitmap[id];
}

bool SuperBlock::isDiskUsed(int id) {
    return diskBitmap[id];
}

bool SuperBlock::isDirUsed(int id) {
    return sfdBitmap[id];
}
