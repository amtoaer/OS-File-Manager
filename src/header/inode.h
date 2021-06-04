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

class Dinode {            //i结点
private:
    RWCT rwct;          //读写控制表
    int i_size;          //文件大小
    int diskblock_num;   //文件占用磁盘块个数
    int diskblock_id;    //第一块磁盘块id
    time_t mtime;        //文件最后修改时间
    time_t ctime;        //文件创建时间
public:
    Dinode() {
        i_size = 0;
        diskblock_num = 1;   //默认一块，存放索引
    }

    void setRwct(RWCT rwct) {
        this->rwct = rwct;
    }

    bool canChangePower(int userId) {    //用户可否更改权限
        //userId为操作用户
        if (rwct.user_id == userId) {
            return true;
        }
        return false;
    }

    void changePower(int userId, string jur) {  //将目标用户权限更改为jur
        //userId为被操作用户
        rwct.r_group.remove(userId);
        rwct.w_group.remove(userId);
        rwct.raw_group.remove(userId);
        rwct.null_group.remove(userId);
        if (jur == "or") {
            rwct.r_group.push_back(userId);
            return;
        }
        if (jur == "ow") {
            rwct.w_group.push_back(userId);
            return;
        }
        if (jur == "raw") {
            rwct.raw_group.push_back(userId);
            return;
        }
        if (jur == "no") {
            rwct.null_group.push_back(userId);
            return;
        }
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

    RWCT getRWCT() {
        return rwct;
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

    //转换为字符串
    string toString();

    //当前用户是否能写文件
    bool canWrite(int id);

    //当前用户是否能读文件
    bool canRead(int id);


};

#endif //OS_FILE_MANAGER_INODE_H
