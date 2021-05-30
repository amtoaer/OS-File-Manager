//
// Created by amtoaer on 2021/5/28.
//

#ifndef OS_FILE_MANAGER_CONFIG_H
#define OS_FILE_MANAGER_CONFIG_H

#define BLOCKSIZE  51*sizeof(int)  //每块大小(字节)
#define BLOCKIDNUM  51 //磁盘块能存的索引数
#define USERNUM  8      //用户最大数量
#define NICFREE  50     //超级块中空闲块数组的最大块数
#define DINODENUM 64     //i结点总数
#define DISKNUM 512      //磁盘总数
#define SFDNUM   128       //目录总数
#define DIRNUM  50         //每个目录下允许嵌套的最大数
#define FILETYPE    1       //目录项为文件类型
#define DIRTYPE     2       //目录项为目录类型

#endif //OS_FILE_MANAGER_CONFIG_H
