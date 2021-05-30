//
// Created by amtoaer on 2021/5/28.
//

#include "../header/system.h"
#include "../header/view.h"
#include <bits/stdc++.h>

using namespace std;

FileSystem::FileSystem() {

}

void FileSystem::format() {
    //格式化超级块
    superBlock.format();
}

bool FileSystem::mkdir(string dir) {
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

bool FileSystem::writeFile(string filePath, string content) {
    int inode_id = findFile(filePath);
    if (inode_id < 0) {
        cout << "文件不存在，文件查找失败!" << endl;
        return false;
    }

    int len = content.length();
    int block_num = ceil(len / (float) BLOCKSIZE);
    if (block_num > superBlock.getFreeDiskNum()) {    //超过磁盘最大数
        cout << "磁盘空间不足!" << endl;
        return false;
    }
    if (block_num <= BLOCKIDNUM - 2) {    //一级索引

    } else if (block_num <= 2 * BLOCKIDNUM - 2) { //二级索引

    } else {  //三级索引

    }
    return true;
}

bool FileSystem::cp(string from, string to) {
    return true;
}

bool FileSystem::mv(string from, string to) {
    return true;
}

bool FileSystem::rm(string filePath, bool isRecursive) {
    return true;
}

vector<string> FileSystem::split(string str, string pattern) {
    string::size_type pos;
    vector<string> result;
    str += pattern;
    int size = str.size();

    for (int i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            string s = str.substr(i, pos - i);
            if (s.length() != 0) {
                result.push_back(s);
            }
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

int FileSystem::findDir(string path) {
    if (path.length() == 0) return -1;
    extern View view;
    if (path[0] != '/') {   //相对路径
        path = view.cur_path + '/' + path;
    }
    vector<string> dirs = split(path, "/");

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