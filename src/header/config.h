//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_CONFIG_H
#define OS_FILE_MANAGER_CONFIG_H

#define BLOCKSIZE  32  //每块大小(字节)
#define USERNUM  8      //用户最大数量
#define NICFREE  50     //超级块中空闲块数组的最大块数
#define DINODENUM 64     //i结点总数
#define DISKNUM 512      //磁盘总数
#define SFDNUM   128       //目录总数
#define DIRNUM  50         //每个目录下允许嵌套的最大数

#endif //OS_FILE_MANAGER_CONFIG_H
