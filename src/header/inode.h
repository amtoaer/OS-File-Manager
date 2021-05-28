//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_INODE_H
#define OS_FILE_MANAGER_INODE_H

#include <bits/stdc++.h>

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

#endif //OS_FILE_MANAGER_INODE_H
