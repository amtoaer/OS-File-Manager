//
// Created by lenovo on 2021-05-30.
//
#include "../header/dir.h"

bool SFD::addItem(SFD_ITEM sfd_item) {
    if (num >= DIRNUM) return false;
    dir[num++] = sfd_item;
    return true;
}

int SFD::hasNext(string name) {
    for (int i = 0; i < num; i++) {
        if (dir[i].name == name) {
            return dir[i].type;
        }
    }
    return 0;
}

SFD_ITEM SFD::removeNext(string name) {
    for (int i = 0; i < num; i++) {
        if (dir[i].name == name) {
            auto result = dir[i];
            for (int j = i + 1; j < num; j++) {
                dir[j - 1] = dir[j];
            }
            num--;
            return result;
        }
    }
    // 需要确保文件/文件夹已存在才可调用该方法，否则会返回新的SFD_ITEM
    return SFD_ITEM();
}

int SFD::getNextDir(string dirname) {
    int sfd_id = -1;
    SFD_ITEM item;
    for (int i = 0; i < num; i++) {
        item = dir[i];
        if (item.type == DIRTYPE && item.name == dirname) {
            sfd_id = item.id;
            break;
        }
    }
    return sfd_id;
}

int SFD::getFileInode(string filename) {
    int inode_id = -1;
    SFD_ITEM item;
    for (int i = 0; i < num; i++) {
        item = dir[i];
        if (item.type == FILETYPE && item.name == filename) {
            inode_id = item.id;
            break;
        }
    }
    return inode_id;
}