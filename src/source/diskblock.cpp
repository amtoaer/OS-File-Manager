//
// Created by lenovo on 2021-05-30.
//

#include "../header/diskblock.h"

void DiskBlock::writeText(string text) {
    if (text.length() > BLOCKSIZE - 1) {
        strncpy(file_cont.doc.content, text.c_str(), BLOCKSIZE - 1);
        file_cont.doc.n = BLOCKSIZE - 1;
    } else {
        strcpy(file_cont.doc.content, text.c_str());
        file_cont.doc.n = text.length();
    }
}

void DiskBlock::getText(char *str) {
    strcpy(str, file_cont.doc.content);
}

bool DiskBlock::addIndex(int index) {
    if (file_cont.index.n >= BLOCKIDNUM) {    //文件满 添加失败
        return false;
    }
    file_cont.index.p[file_cont.index.n++] = index;
    return true;
}

void DiskBlock::copy(int block[]) {
    this->file_cont.index.n = block[0];
    for (int i = 1; i <= NICFREE; i++) {
        this->file_cont.index.p[i - 1] = block[i];
    }
}

int DiskBlock::getDinodeId(int nth) {
    if (nth >= file_cont.index.n)
        return -1;
    return file_cont.index.p[nth];
}

void DiskBlock::clearIndex() {
    file_cont.index.n = 0;
}
