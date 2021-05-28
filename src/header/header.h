//
// Created by amtoaer on 2021/5/27.
//

#ifndef OS_FILE_MANAGER_HEADER_H
#define OS_FILE_MANAGER_HEADER_H

#include <bits/stdc++.h>

using namespace std;

#define BLOCKSIZE  512  //每块大小(字节)
#define USERNUM  8      //用户最大数量
#define NICFREE  50     //超级块中空闲块数组的最大块数
#define DINODENUM 64     //i结点总数
#define DISKNUM 512      //磁盘总数
#define SFDNUM   128       //目录总数

extern int UserN = 0;   //当前已有用户

struct User {      //用户
    int id;     //用户id号
    string username;
    string userpwd;
    int usergrp;//用户组
    bool isManager; //管理员
    User() {}

    User(string name, string pwd, int grp, bool isM) {
        id = UserN++;
        username = name;
        userpwd = pwd;
        usergrp = grp;
        isManager = isM;
    }
};

struct Doc {
    char content[BLOCKSIZE];    //文件内容
    int n;        //已占空间
};

struct Index {
    int p[BLOCKSIZE / 4]; //索引集合
    int n;  //索引总数
};

union DisBlock { //磁盘块
    Doc doc;
    Index index;
};
struct DiskBlock {      //磁盘块
    char content[BLOCKSIZE];    //文件内容
    int n;        //已占空间
};

struct RWCT {    //读写控制块
    int user_id;    //属主id
    vector<int> read_group; //只读组 存放用户id
    vector<int> raw_group;  //只写 存放用户id
    vector<int> invisible_group;    //不可见组 存放用户id
};

struct Inode {            //内存i结点
    RWCT rwct;          //读写控制表
    int i_size;            //文件大小
    int diskblock_num;  //文件占用磁盘块个数
    int diskblock_id;   //第一块磁盘块id
    int i_count;        //文件的引用数
    char i_mtime[15];  //文件最后修改时间
    char i_ctime[15];  //文件创建时间
    //关联文件数? 其他属性？

};

struct Dinode {            //i结点
    RWCT rwct;          //读写控制表
    int i_size;            //文件大小
    int diskblock_num;  //文件占用磁盘块个数
    int diskblock_id;   //第一块磁盘块id

    char i_mtime[15];  //文件最后修改时间
    char i_ctime[15];  //文件创建时间
    //关联文件数? 其他属性？

};

struct SuperBlock {         //超级块结构
    int freeinode_num;      //空闲i结点总数
    vector<int> freeInode; //空闲i结点id数组

    int freedisk_num;        //空闲磁盘块块数
    int freeDiskStack[NICFREE + 1]; //成组链接空闲磁盘块栈
};

struct SFD_ITEM {
    int type;       //目录 或 文件
    string name;    //命名
    int id;         //目录：下一级目录索引  文件：磁盘i结点
};

struct MFD_ITEM {
    int userid;   //用户名
    int dir_id; //目录id
};

struct SFD {
    int num;
    vector<SFD_ITEM> dir;

    SFD() {
        num = 0;
    }
};

struct BFD {
    int ffd_num;        //空闲目录数
    vector<int> FFD;    //空闲目录id

    int mfd_num;          //用户总数
    vector<MFD_ITEM> MFD; //用户目录id

};

extern FileSystem FS;    //文件系统
extern bool diBitmap[DINODENUM];    //磁盘i结点位示图
extern bool diskBitmap[DISKNUM];    //磁盘块结点位示图
extern bool sfdBitmap[SFDNUM];      //目录位示图
extern int sfd_id;      //当前目录id
extern User Users[USERNUM];  //全部用户
extern User cur_user;  //当前用户

#endif //OS_FILE_MANAGER_HEADER_H
