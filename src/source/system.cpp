//
// Created by amtoaer on 2021/5/28.
//

#include "../header/system.h"
#include "../header/view.h"
#include "../header/utils.h"
#include <bits/stdc++.h>

using namespace std;

FileSystem::FileSystem() {

}

void FileSystem::format() {
    //格式化超级块
    superBlock.format();
}

string FileSystem::getFullPath(string path) {
    if (path.length() == 0) {
        return path;
    }
    if (path[0] != '/') {   //相对路径
        return view.cur_path + '/' + path;
    }
    return path;
}

bool FileSystem::mkdir(string dir) {
    auto dirs = split(getFullPath(dir), "/");
    if (dirs.size() == 0) {
        // 输入为空
        return false;
    }
    // 保存需创建的文件夹名
    string dirToMake = dirs.back();
    dirs.pop_back();
    int location = findDir(dirs);
    if (location == -1) {
        // 父路径不存在
        return false;
    }
    if (sfd[location].getNextDir(dirToMake) != -1) {
        // 同名文件夹已存在
        return false;
    }
    int freeDirID = sb.getFreeDir();
    if (freeDirID == -1) {
        // 无空闲目录
        return false;
    }
    bool result = sfd[location].addItem(SFD_ITEM(2, dirToMake, freeDirID));
    if (!result) {
        // 当前目录下的item已满
        return false;
    }
    return true;
}

bool FileSystem::touch(string filePath) {
    int cur_dir_id = findDir(filePath);
    if (cur_dir_id < 0) {
        cout << "目录不存在，文件创建失败!" << endl;
        return false;
    }

    if (!sfd[cur_dir_id].canAddItem()) {
        cout << "当前目录下文件达到上限，文件创建失败!" << endl;
        return false;
    }

    //成组链接申请空闲磁盘块############
    int diskblock_id = 0;
    //当分配失败时，返回false

    //分配i结点
    if (superBlock.getFreeInode() <= 0) {
        cout << "i结点已用完，文件创建失败!" << endl;
        return false;
    }
    int inode_id = superBlock.getFreeInode();

    diNode[inode_id].setDiskBlockId(diskblock_id);  //设置初始快

    //读写控制 目前只添加了文件属主 其余用户只能读 属主可读写
    RWCT rwct;
    extern View view;
    rwct.user_id = view.cur_user.getId();
    extern UserManage user_mag;
    vector<int> user_id = user_mag.getUserId();
    for (int id:user_id) {
        if (id != view.cur_user.getId())
            rwct.r_group.push_back(id);
    }
    rwct.raw_group.push_back(view.cur_user.getId());
    diNode[inode_id].setRwct(rwct);

    SYSTEMTIME m_time;  //添加创建时间 修改时间
    GetLocalTime(&m_time);
    char szDateTime[30] = {0};
    sprintf(szDateTime, "%02d-%02d-%02d %02d:%02d:%02d", m_time.wYear, m_time.wMonth,
            m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond);
    diNode[inode_id].setCreatedTime(szDateTime);
    diNode[inode_id].setModifiedTime(szDateTime);

    string filename;
    cout << "请输入文件名:";
    cin >> filename;

    SFD_ITEM new_file(FILETYPE, filename, inode_id);    //添加至目录下
    sfd[cur_dir_id].addItem(new_file);

    return true;
}

//bool FileSystem::writeFile(string filePath, string content) {
//    int inode_id = findFile(filePath);
//    if (inode_id < 0) {
//        cout << "文件不存在，文件查找失败!" << endl;
//        return false;
//    }
//
//    int len = content.length();
//    int block_num = ceil(len / (float) BLOCKSIZE);
//    if (block_num > superBlock.getFreeDiskNum()) {    //超过磁盘最大数
//        cout << "磁盘空间不足!" << endl;
//        return false;
//    }
//    if (block_num <= BLOCKIDNUM - 2) {    //一级索引
//
//    } else if (block_num <= 2 * BLOCKIDNUM - 2) { //二级索引
//
//    } else {  //三级索引
//
//    }
//    return true;
//}

bool FileSystem::cp(string from, string to) {
    return true;
}

bool FileSystem::mv(string from, string to) {
    return true;
}

bool FileSystem::rm(string filePath, bool isRecursive) {
    return true;
}

int FileSystem::findDir(vector<string> dirs) {
    int cur_dir_id = root_id;
    int next_dir_id;
    for (string dirname:dirs) {
        next_dir_id = sfd[cur_dir_id].getNextDir(dirname);
        if (next_dir_id < 0) {  //目录不存在
            return -1;
        }
        cur_dir_id = next_dir_id;
    }
    return cur_dir_id;
}

int FileSystem::findDir(string path) {
    return findDir(split(getFullPath(path), "/"));
}

int FileSystem::findFile(string path) {
    if (path.length() == 0) return -1;
    extern View view;
    if (path[0] != '/') {   //相对路径
        path = view.cur_path + '/' + path;
    }
    vector<string> strs = split(path, "/");

    if (strs.size() == 0) {
        return -1;
    }
    int cur_dir_id = root_id;
    int next_dir_id;
    for (int i = 0; i < strs.size() - 1; i++) {
        next_dir_id = sfd[cur_dir_id].getNextDir(strs[i]);
        if (next_dir_id < 0) {  //目录不存在
            return -1;
        }
        cur_dir_id = next_dir_id;
    }
    int inode_id = sfd[cur_dir_id].getFileInode(strs[strs.size() - 1]);
    if (inode_id < 0) {
        return -1;
    }
    return inode_id;
}

DiskBlock FileSystem::getDiskBlock(int index) {
    return this->diskBlock[index];
}