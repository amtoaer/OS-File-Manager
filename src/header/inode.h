//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_INODE_H
#define OS_FILE_MANAGER_INODE_H

#include <bits/stdc++.h>
#include <time.h>

using namespace std;

struct RWCT {    //读写控制块
    int user_id;    //属主id
    list<int> r_group; //只读组 存放用户id
    list<int> w_group;  //只写组
    list<int> raw_group;  //读写组
    list<int> null_group;   //无权限组
};

//暂时还没想到内存Inode能做什么
class Inode {            //内存i结点
private:
    RWCT rwct;          //读写控制表
    int i_size;            //文件大小
    int diskblock_num;  //文件占用磁盘块个数
    int diskblock_id;   //第一块磁盘块id
    int i_count;        //文件的引用数
    char i_mtime[20];  //文件最后修改时间
    char i_ctime[20];  //文件创建时间
    //关联文件数? 其他属性？
};

class Dinode {            //磁盘i结点
private:
    RWCT rwct;          //读写控制表
    int i_size;            //文件大小
    int diskblock_num;  //文件占用磁盘块个数
    int diskblock_id;   //第一块磁盘块id
    time_t mtime;   //文件最后修改时间
    time_t ctime;   //文件创建时间
    //关联文件数? 其他属性？
public:
    Dinode() {
        i_size = 0;
        diskblock_num = 1;   //默认一块，存放索引
    }

    void setRwct(RWCT rwct) {
        this->rwct = rwct;
    }

    void setSize(int size) {
        i_size = size;
    }

    void setDiskBlockNum(int diskblock_num) {
        this->diskblock_num = diskblock_num;
    }

    void setDiskBlockId(int diskblock_id) {
        this->diskblock_id = diskblock_id;
    }

    void setModifiedTime(time_t new_time) {
        mtime = new_time;
    }

    void setCreatedTime(time_t created_time) {
        ctime = created_time;
    }

    int getSize() {
        return i_size;
    }

    int getDiskblockNum() {
        return diskblock_num;
    }

    int getDiskblockId() {
        return diskblock_id;
    }

    time_t getModifiedTime() {
        return mtime;
    }

    time_t getCreatedTime() {
        return ctime;
    }

};

#endif //OS_FILE_MANAGER_INODE_H
