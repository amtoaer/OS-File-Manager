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
    root_id = 0;
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
    /*
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
    int two_index_start = 2 * BLOCKIDNUM + 1;//二次间址

    int direct_disk_id; //直接索引磁盘块编号
    int one_disk_id;    //一级间址磁盘块编号
    int two_disk_id;    //二级间址磁盘块编号

    int write_start = 0;
    //写入内容到磁盘块
    for (int i = 0; i < block_num - 1; i++) {
        if (i == direct_index) {
            diNode[inode_id].setSize(len);
            diNode[inode_id].setDiskBlockNum(block_num);
            diNode[inode_id].setDiskBlockId(applied_disk[i]);
            direct_disk_id = applied_disk[i];
            diskBlock[direct_disk_id].clearIndex();
            //设置修改时间
            SYSTEMTIME m_time;
            GetLocalTime(&m_time);
            char szDateTime[30] = {0};
            sprintf(szDateTime, "%02d-%02d-%02d %02d:%02d:%02d", m_time.wYear, m_time.wMonth,
                    m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond);
            diNode[inode_id].setModifiedTime(szDateTime);
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
        } else {

        }
    }
    */
    /*
    int index_disk_id = diNode[inode_id].getDiskblockId();
    for (int i = 0; i < block_num - 1; i++) {
        if (i < BLOCKIDNUM - 2) {
            int diskblock_id = 0;  //成组链接获取###########
            string subs = content.substr(i * BLOCKSIZE, BLOCKSIZE);
            strcpy(diskBlock[diskblock_id].doc.content, subs.data());
            diskBlock[diskblock_id].doc.n = BLOCKSIZE;

        }
    }

    if (block_num <= BLOCKIDNUM - 2) {    //一级索引


    } else if (block_num <= 2 * BLOCKIDNUM - 2) { //二级索引

    } else {  //三级索引

    }
    */

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