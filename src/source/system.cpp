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
    root_id = 0;
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

    //成组链接申请空闲磁盘块
    int diskblock_id = superBlock.getFreeDiskBlock();
    if (diskblock_id < 0) {
        cout << "磁盘空间不足，文件创建失败!" << endl;
        return false;
    }

    //分配i结点
    int inode_id = superBlock.getFreeInode();
    if (inode_id <= 0) {
        cout << "i结点已用完，文件创建失败!" << endl;
        return false;
    }

    //设置初始块
    diNode[inode_id].setDiskBlockId(diskblock_id);

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

    //添加创建时间 修改时间
    time_t timep;
    time(&timep);
    diNode[inode_id].setCreatedTime(timep);
    diNode[inode_id].setModifiedTime(timep);

    string filename;
    cout << "请输入文件名:";
    cin >> filename;

    //添加至目录下
    SFD_ITEM new_file(FILETYPE, filename, inode_id);
    sfd[cur_dir_id].addItem(new_file);

    return true;
}

bool FileSystem::writeFile(string filePath, string content) {
    int inode_id = findFile(filePath);
    if (inode_id < 0) {
        cout << "文件不存在，文件查找失败!" << endl;
        return false;
    }

    //申请磁盘空间
    int len = content.length();
    int block_num = calculateDiskNum(len);
    int applied_block_num = diNode[inode_id].getDiskblockNum();
    if (block_num > superBlock.getFreeDiskNum() + applied_block_num) {    //超过磁盘最大数
        cout << "磁盘空间不足!" << endl;
        return false;
    }
    vector<int> applied_disk = getFileDiskIds(inode_id);
    int pre_n = applied_disk.size();
    //已申请空间不足
    while (applied_disk.size() < block_num) {
        int new_inode_id = superBlock.getFreeDiskBlock();
        if (new_inode_id < 0) {
            cout << "磁盘分配出错，文件写入失败!" << endl;
            //出错 回收申请的磁盘块
            while (applied_disk.size() > pre_n) {
                int release_inode_id = applied_disk.back();
                applied_disk.pop_back();
                superBlock.recycleDiskBlock(release_inode_id);
            }
            return false;
        }
        applied_disk.push_back(new_inode_id);
    }
    //已申请空间剩余
    while (applied_disk.size() > block_num) {
        int release_inode_id = applied_disk.back();
        applied_disk.pop_back();
        superBlock.recycleDiskBlock(release_inode_id);
    }

    int direct_index = 0; //直接索引块编号
    int one_index = BLOCKIDNUM - 1; //一次间址索引块编号
    int two_index = 2 * BLOCKIDNUM;//二次间址索引块编号

    int direct_disk_id; //直接索引磁盘块编号
    int one_disk_id;    //一级间址磁盘块编号
    int two_disk_id;    //二级间址磁盘块编号
    int two_disk_nth;   //二级间址索引磁盘块编号

    int write_start = 0;
    //写入内容到磁盘块
    for (int i = 0; i < block_num; i++) {
        if (i == direct_index) {
            diNode[inode_id].setSize(len);
            diNode[inode_id].setDiskBlockNum(block_num);
            diNode[inode_id].setDiskBlockId(applied_disk[i]);
            direct_disk_id = applied_disk[i];
            diskBlock[direct_disk_id].clearIndex();
            //设置修改时间
            time_t timep;
            time(&timep);
            diNode[inode_id].setModifiedTime(timep);
        } else if (i < one_index) { //直接索引
            string substr = content.substr(write_start, BLOCKSIZE - 1);
            int disk_id = applied_disk[i];
            diskBlock[disk_id].writeText(substr);
            write_start += BLOCKSIZE - 1;
            diskBlock[direct_disk_id].addIndex(disk_id);
        } else if (i == one_index) {
            one_disk_id = applied_disk[i];
            diskBlock[direct_disk_id].addIndex(one_disk_id);
            diskBlock[one_disk_id].clearIndex();
        } else if (i < two_index) { //一次间址
            string substr = content.substr(write_start, BLOCKSIZE - 1);
            int disk_id = applied_disk[i];
            diskBlock[disk_id].writeText(substr);
            write_start += BLOCKSIZE - 1;
            diskBlock[one_disk_id].addIndex(disk_id);
        } else if (i == two_index) {
            two_disk_id = applied_disk[i];
            diskBlock[direct_disk_id].addIndex(two_disk_id);
            diskBlock[two_disk_id].clearIndex();
        } else if ((i - two_index - 1) % (BLOCKIDNUM + 1) == 0) { //二次间址的索引
            two_disk_nth = applied_disk[i];
            diskBlock[two_disk_id].addIndex(two_disk_nth);
            diskBlock[two_disk_nth].clearIndex();
        } else {
            string substr = content.substr(write_start, BLOCKSIZE - 1);
            int disk_id = applied_disk[i];
            diskBlock[disk_id].writeText(substr);
            write_start += BLOCKSIZE - 1;
            diskBlock[two_disk_nth].addIndex(disk_id);
        }
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

DiskBlock FileSystem::getDiskBlock(int index) {
    return this->diskBlock[index];
}

vector<int> FileSystem::getFileDiskIds(int inode_id) {
    vector<int> res;
    int block_num = diNode[inode_id].getDiskblockNum();
    int block_id = diNode[inode_id].getDiskblockId();
    res.push_back(block_id);

    if (block_num <= BLOCKIDNUM - 1) {   //直接寻址
        for (int i = 0; i < block_num - 1; i++) {
            res.push_back(diskBlock[block_id].getDinodeId(i));
        }
    } else if (block_num <= 2 * BLOCKIDNUM) {    //一次间址
        for (int i = 0; i < BLOCKIDNUM - 2; i++) {
            res.push_back(diskBlock[block_id].getDinodeId(i));
        }
        int next_block_id = diskBlock[block_id].getDinodeId(BLOCKIDNUM - 2);
        for (int i = 0; i < block_num - BLOCKIDNUM; i++) {
            res.push_back(diskBlock[next_block_id].getDinodeId(i));
        }
    } else {   //二次间址
        for (int i = 0; i < BLOCKIDNUM - 2; i++) {
            res.push_back(diskBlock[block_id].getDinodeId(i));
        }
        int next_block_id = diskBlock[block_id].getDinodeId(BLOCKIDNUM - 2);
        for (int i = 0; i < BLOCKIDNUM; i++) {
            res.push_back(diskBlock[next_block_id].getDinodeId(i));
        }
        next_block_id = diskBlock[block_id].getDinodeId(BLOCKIDNUM - 1);
        int k = 0;
        for (; k < (block_num - 2 * BLOCKIDNUM - 1) / (BLOCKIDNUM + 1); k++) {
            int nn_block_id = diskBlock[next_block_id].getDinodeId(k);
            for (int i = 0; i < BLOCKIDNUM; i++) {
                res.push_back(diskBlock[nn_block_id].getDinodeId(i));
            }
        }
        int rest_num = (block_num - 2 * BLOCKIDNUM - 1) % (BLOCKIDNUM + 1);
        int nn_block_id = diskBlock[next_block_id].getDinodeId(k);
        for (int i = 0; i < rest_num - 1; i++) {
            res.push_back(diskBlock[nn_block_id].getDinodeId(i));
        }
    }
    return res;
}

int FileSystem::calculateDiskNum(int len) {
    int n;  //文件占用磁盘数
    int file_num = ceil(len / (double) (BLOCKSIZE - 1));
    if (file_num <= BLOCKIDNUM - 2) {
        n = file_num + 1;
    } else if (file_num <= 2 * BLOCKIDNUM - 2) {
        n = file_num + 2;
    } else {
        int a = ceil((file_num - 2 * BLOCKIDNUM + 2) / (double) (BLOCKIDNUM));
        n = file_num + 3 + a;
    }
    return n;
}