//
// Created by amtoaer on 2021/5/28.
//

#include "../header/system.h"
#include "../header/view.h"
#include "../header/utils.h"
#include <bits/stdc++.h>
#include <fstream>

using namespace std;

FileSystem::FileSystem() {
    if (!readFromFile()) {
        this->format();
    }
}

void FileSystem::format() {
    //格式化超级块
    superBlock.format();
    root_id = 0;
}

bool FileSystem::changeFileRWCT(string filePath, string userName, string jurisdiction) {
    //路径filePath精确到文件名，jurisdiction为修改后的权限名称
    //userName为被修改权限的用户名
    //修改后的权限名称只允许为：or、ow、raw、no
    if (jurisdiction != "or" && jurisdiction != "ow" && jurisdiction != "raw" && jurisdiction != "no") {
        //输入权限有误
        return false;
    }
    int goalUserId = user_mag.getId(userName);
    vector<int> allUserId = user_mag.getUserId();
    bool found = false; //是否找到了目标用户
    for (int userId: allUserId) {
        if (userId == goalUserId) {
            found = true;
            break;
        }
    }
    if (!found) {
        //没有可操作的目标用户
        return false;
    }
    auto fromDirs = split(getFullPath(filePath), "/");
    string fileName = fromDirs.back();
    fromDirs.pop_back();

    int fromLocation = findDir(fromDirs);
    int fileId = sfd[fromLocation].getFileInode(fileName);
    if (fileId == -1) {
        //不存在该文件
        return false;
    }
    if (!diNode[fileId].canChangePower(view.cur_user.getId())) {
        //该文件不可被当前用户修改权限
        return false;
    }
    diNode[fileId].changePower(goalUserId, jurisdiction);
    return true;
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
    int freeDirID = superBlock.getFreeDir();
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
    string dirpath, filename;
    //划分目录和文件
    int split_id = filePath.length() - 1;
    for (; split_id >= 0; split_id--) {
        if (filePath[split_id] == '/')
            break;
    }
    dirpath = filePath.substr(0, split_id);
    filename = filePath.substr(split_id + 1, filePath.length());

    int cur_dir_id = findDir(dirpath);
    if (cur_dir_id < 0) {
        cout << "目录不存在，文件创建失败!" << endl;
        return false;
    }

    //检查重名问题
    if (sfd[cur_dir_id].hasNext(filename) != 0) {
        cout << "此位置包含同名文件或目录，文件创建失败!" << endl;
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
    diskBlock[diskblock_id].setType(INDEXTYPE);
    diskBlock[diskblock_id].clearIndex();

    //分配i结点
    int inode_id = superBlock.getFreeInode();
    if (inode_id < 0) {
        cout << "i结点已用完，文件创建失败!" << endl;
        //回收磁盘块
        superBlock.recycleDiskBlock(diskblock_id);
        return false;
    }

    //初始化i结点
    diNode[inode_id].setSize(0);
    diNode[inode_id].setDiskBlockNum(1);
    diNode[inode_id].setDiskBlockId(diskblock_id);

    //读写控制 目前只添加了文件属主 其余用户只能读 属主可读写
    RWCT rwct;
    rwct.user_id = view.cur_user.getId();
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

    //添加至目录下
    SFD_ITEM new_file(FILETYPE, filename, inode_id);
    sfd[cur_dir_id].addItem(new_file);

    return true;
}

void FileSystem::writeToDiskBlock(vector<int> applied_disk, int inode_id, int start_block, string content) {
    int direct_index = 0; //直接索引块编号
    int one_index = BLOCKIDNUM - 1; //一次间址索引块编号
    int two_index = 2 * BLOCKIDNUM;//二次间址索引块编号

    int direct_disk_id; //直接索引磁盘块编号
    int one_disk_id;    //一级间址磁盘块编号
    int two_disk_id;    //二级间址磁盘块编号
    int two_disk_nth;   //二级间址索引磁盘块编号

    int block_num = applied_disk.size();
    if (start_block > direct_index) {
        direct_disk_id = applied_disk[direct_index];
    }
    if (start_block > one_index) {
        one_disk_id = applied_disk[one_index];
    }
    if (start_block > two_index) {
        two_disk_id = applied_disk[two_index];
    }
    if (start_block > two_index + 1) {
        for (int i = start_block; i >= two_index + 1; i--) {
            if (i % (two_index + 1) == 0) {
                two_disk_nth = applied_disk[i];
                break;
            }
        }
    }
    int write_start = 0;

    //写入内容到磁盘块
    for (int i = start_block; i < block_num; i++) {
        if (i == direct_index) {
            diNode[inode_id].setSize(content.length());
            diNode[inode_id].setDiskBlockNum(block_num);
            diNode[inode_id].setDiskBlockId(applied_disk[i]);
            direct_disk_id = applied_disk[i];
            diskBlock[direct_disk_id].clearIndex();
            diskBlock[direct_disk_id].setType(INDEXTYPE);
            //设置修改时间
            time_t timep;
            time(&timep);
            diNode[inode_id].setModifiedTime(timep);
        } else if (i < one_index) { //直接索引
            string substr = content.substr(write_start, BLOCKSIZE - 1);
            int disk_id = applied_disk[i];
            diskBlock[disk_id].writeText(substr);
            diskBlock[disk_id].setType(TEXTTYPE);
            write_start += BLOCKSIZE - 1;
            if (i != start_block)
                diskBlock[direct_disk_id].addIndex(disk_id);
        } else if (i == one_index) {
            one_disk_id = applied_disk[i];
            diskBlock[direct_disk_id].addIndex(one_disk_id);
            diskBlock[one_disk_id].clearIndex();
            diskBlock[one_disk_id].setType(INDEXTYPE);
        } else if (i < two_index) { //一次间址
            string substr = content.substr(write_start, BLOCKSIZE - 1);
            int disk_id = applied_disk[i];
            diskBlock[disk_id].writeText(substr);
            diskBlock[disk_id].setType(TEXTTYPE);
            write_start += BLOCKSIZE - 1;
            if (i != start_block)
                diskBlock[one_disk_id].addIndex(disk_id);
        } else if (i == two_index) {
            two_disk_id = applied_disk[i];
            diskBlock[direct_disk_id].addIndex(two_disk_id);
            diskBlock[two_disk_id].clearIndex();
            diskBlock[two_disk_id].setType(INDEXTYPE);
        } else if ((i - two_index - 1) % (BLOCKIDNUM + 1) == 0) { //二次间址的索引
            two_disk_nth = applied_disk[i];
            diskBlock[two_disk_id].addIndex(two_disk_nth);
            diskBlock[two_disk_nth].clearIndex();
            diskBlock[two_disk_nth].setType(INDEXTYPE);
        } else {
            string substr = content.substr(write_start, BLOCKSIZE - 1);
            int disk_id = applied_disk[i];
            diskBlock[disk_id].writeText(substr);
            diskBlock[disk_id].setType(TEXTTYPE);
            write_start += BLOCKSIZE - 1;
            if (i != start_block)
                diskBlock[two_disk_nth].addIndex(disk_id);
        }
    }

}

bool FileSystem::writeFile(string filePath, string content) {
    int inode_id = findFile(filePath);
    if (inode_id < 0) {
        cout << "文件不存在，文件查找失败!" << endl;
        return false;
    }
    if (!diNode[inode_id].canWrite(view.cur_user.getId())) {
        //无写权限
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

    //将内容写到磁盘块
    writeToDiskBlock(applied_disk, inode_id, 0, content);
    return true;
}

bool FileSystem::appendToFile(string filePath, string content) {
    int inode_id = findFile(filePath);
    if (inode_id < 0) {
        cout << "文件不存在，文件查找失败!" << endl;
        return false;
    }

    if (!diNode[inode_id].canWrite(view.cur_user.getId())) {
        //无写权限
        return false;
    }

    //申请磁盘空间
    int file_len = diNode[inode_id].getSize() + content.length();
    int block_num = calculateDiskNum(file_len);
    int extra_block_num = block_num - diNode[inode_id].getDiskblockNum();
    if (extra_block_num > superBlock.getFreeDiskNum()) {
        cout << "磁盘空间不足，追加失败!" << endl;
        return false;
    }
    vector<int> applied_disk = getFileDiskIds(inode_id);
    int start_block = applied_disk.size() - 1;
    while (applied_disk.size() < block_num) {
        int new_inode_id = superBlock.getFreeDiskBlock();
        if (new_inode_id < 0) {
            cout << "磁盘分配出错，文件写入失败!" << endl;
            //出错 回收申请的磁盘块
            while (applied_disk.size() > start_block + 1) {
                int release_inode_id = applied_disk.back();
                applied_disk.pop_back();
                superBlock.recycleDiskBlock(release_inode_id);
            }
            return false;
        }
        applied_disk.push_back(new_inode_id);
    }

    if (start_block > 0) {  //原文件最后一块有内容
        //更新inode信息
        diNode[inode_id].setSize(file_len);
        diNode[inode_id].setDiskBlockNum(block_num);
        time_t timep;
        time(&timep);
        diNode[inode_id].setModifiedTime(timep);

        char temp[BLOCKSIZE + 1];
        diskBlock[applied_disk[start_block]].getText(temp);
        content = string(temp) + content;
    }
    writeToDiskBlock(applied_disk, inode_id, start_block, content);
    return true;
}

string FileSystem::readFile(string filePath) {
    int inode_id = findFile(filePath);
    if (inode_id < 0) {
        return "";
    }

    if (!diNode[inode_id].canRead(view.cur_user.getId())) {
        //无读权限
        return "";
    }

    vector<int> diskblock_id = getFileContentDiskIds(inode_id);
    string content = "";
    char temp[BLOCKSIZE + 1];
    for (int id:diskblock_id) {
        diskBlock[id].getText(temp);
        string substr(temp);
        content += substr;
    }
    return content;
}

bool FileSystem::cp(string from, string to) {
    auto fromDirs = split(getFullPath(from), "/");
    auto toDirs = split(getFullPath(to), "/");
    if (fromDirs.size() == 0/* || toDirs.size() == 0*/) {//复制的文件或者目的目录未空
        return false;
    }

    // 需复制的 文件夹名 或者 文件名
    string fileOrDir = fromDirs.back();
    fromDirs.pop_back();

    int fromLocation = findDir(fromDirs);
    if (fromLocation == -1) {
        //父路径不存在
        return false;
    }

    int toLocation = findDir(toDirs);
    if (toLocation == -1) {
        //目的路径不存在
        return false;
    }

    if (!sfd[toLocation].canAddItem()) {
        //目的目录已满
        return false;
    }

    int fileNum = 0, dirNum = 0;

    if (sfd[fromLocation].getNextDir(fileOrDir) == -1) {//该名称不是一个文件夹
        fileNum = 1;
        int fileId = findFile(getFullPath(from));
        if (fileId == -1) {
            //找不到该文件
            return false;
        }
        //单文件进行复制
        string content = readFile(getFullPath(from));
        string toPath = getFullPath(to) + "/" + fileOrDir;
        touch(toPath);
        writeFile(toPath, content);
        return true;
    }

    //需要复制的是一个文件夹

    if (sfd[toLocation].getNextDir(fileOrDir) != -1 || sfd[toLocation].getFileInode(fileOrDir) != -1) {
        //存在重名的文件夹 或者 存在与其重名的文件
        return false;
    }

    dirNum++;
    calculateDirAndFile(sfd[fromLocation].getNextDir(fileOrDir), dirNum, fileNum);

    if (superBlock.getFreeDirNum() < dirNum) {
        //空闲目录数不够
        return false;
    }

    if (superBlock.getFreeInodeNum() < fileNum) {
        //空闲i节点数不够
        return false;
    }
    //在目的目录下创建文件夹
    mkdir(getFullPath(to) + fileOrDir);
    //将要复制的文件夹下的所有内容 复制到 新创建的文件夹下
    cpCurrentDir(from, to + fileOrDir);

    return true;
}

void FileSystem::calculateDirAndFile(int id, int &dirNum, int &fileNum) {
    vector<SFD_ITEM> allNext = sfd[id].getAllNext();
    for (SFD_ITEM next: allNext) {
        if (next.type == 1) {
            //是文件
            fileNum++;
        } else {
            //是文件夹
            dirNum++;
            calculateDirAndFile(next.id, dirNum, fileNum);
        }
    }
}

void FileSystem::cpCurrentDir(string from, string to) {
    /*
    //from精确到文件夹名，to精确到目的文件夹
    // 如/a目录下的b文件夹下的所有内容复制到/c目录下的b文件夹下
    // from = /a/b
    // to = /c/b
    auto fromDirs = split(getFullPath(from), "/");

    int id = findDir(fromDirs);
    vector<SFD_ITEM> allNext = sfd[id].getAllNext();
    for (SFD_ITEM next: allNext) {
        string toPath = getFullPath(to) + "/" + next.name;
        if (next.type == 1) {
            //复制文件
            string content = readFile(getFullPath(from) + next.name);
            touch(getFullPath(to) + "/" + next.name);
            writeFile(toPath, content);
        } else {
            //复制文件夹
            string fromPath = getFullPath(from) + "/" + next.name;
            mkdir(toPath);
            cpCurrentDir(fromPath, toPath);
        }
    }
     */
}

bool FileSystem::mv(string from, string to) {
    // 此处规定: from精确到文件/文件夹，to精确到上级目录
    // 如/a目录下的b文件移动到/c
    // from = /a/b
    // to = /c
    auto fromDirs = split(getFullPath(from), "/");
    auto toDirs = split(getFullPath(to), "/");
    if (fromDirs.size() == 0) {
        // 目录非法
        return false;
    }
    auto name = fromDirs.back();
    fromDirs.pop_back();
    auto location = findDir(fromDirs);
    auto targetLocation = findDir(toDirs);
    if (targetLocation == -1) {
        // 目标文件夹不存在
        return false;
    }
    if (sfd[targetLocation].hasNext(name)) {
        // 目标文件夹已存在同名文件或文件夹
        return false;
    }
    if (location == -1) {
        // from上级目录不存在
        return false;
    }
    if (!sfd[location].hasNext(name)) {
        // from文件或文件夹不存在
        return false;
    }
    auto item = sfd[location].removeNext(name);
    return sfd[targetLocation].addItem(item);
}

bool FileSystem::rm(string filePath) {
    auto dirs = split(getFullPath(filePath), "/");
    if (dirs.size() == 0) {
        // 路径不合法
        return false;
    }
    auto name = dirs.back();
    dirs.pop_back();
    int location = findDir(dirs);
    if (location == -1) {
        // 不存在上级目录
        return false;
    }
    auto has = sfd[location].hasNext(name);
    if (!has) {
        // 不存在该文件或目录
        return false;
    } else if (has == 1) {
        // 是文件，找到inode
        auto inode = sfd[location].getFileInode(name);
        // 通过inode找到磁盘块id并遍历回收
        auto ids = getFileDiskIds(inode);
        for (int i:ids) {
            superBlock.recycleDiskBlock(i);
        }
        // 回收inode
        superBlock.recycleInode(inode);
        // 移除父文件夹的该条记录
        sfd[location].removeNext(name);
    } else {
        // 是目录，找到dir
        auto dir = sfd[location].getNextDir(name);
        // 遍历回收该文件夹下的文件/文件夹
        for (auto i:sfd[dir].getAllNext()) {
            rm(filePath + "/" + i.name);
        }
        // 回收dir
        superBlock.recycleDir(dir);
        // 移除父文件夹的该条记录
        sfd[location].removeNext(name);
    }
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
    int inode_id = sfd[cur_dir_id].getFileInode(strs.back());
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
        res.push_back(next_block_id);
        for (int i = 0; i < block_num - BLOCKIDNUM; i++) {
            res.push_back(diskBlock[next_block_id].getDinodeId(i));
        }
    } else {   //二次间址
        for (int i = 0; i < BLOCKIDNUM - 2; i++) {
            res.push_back(diskBlock[block_id].getDinodeId(i));
        }
        int next_block_id = diskBlock[block_id].getDinodeId(BLOCKIDNUM - 2);
        res.push_back(next_block_id);
        for (int i = 0; i < BLOCKIDNUM; i++) {
            res.push_back(diskBlock[next_block_id].getDinodeId(i));
        }
        next_block_id = diskBlock[block_id].getDinodeId(BLOCKIDNUM - 1);
        res.push_back(next_block_id);
        int k = 0;
        for (; k < (block_num - 2 * BLOCKIDNUM - 1) / (BLOCKIDNUM + 1); k++) {
            int nn_block_id = diskBlock[next_block_id].getDinodeId(k);
            res.push_back(nn_block_id);
            for (int i = 0; i < BLOCKIDNUM; i++) {
                res.push_back(diskBlock[nn_block_id].getDinodeId(i));
            }
        }
        int rest_num = (block_num - 2 * BLOCKIDNUM - 1) % (BLOCKIDNUM + 1);
        if (rest_num > 0) {
            int nn_block_id = diskBlock[next_block_id].getDinodeId(k);
            res.push_back(nn_block_id);
            for (int i = 0; i < rest_num - 1; i++) {
                res.push_back(diskBlock[nn_block_id].getDinodeId(i));
            }
        }
    }
    return res;
}

vector<int> FileSystem::getFileContentDiskIds(int inode_id) {
    vector<int> res;
    vector<int> temp = getFileDiskIds(inode_id);
    int id1 = 0;
    int id2 = BLOCKIDNUM - 1;
    int id3 = 2 * BLOCKIDNUM;
    int id4 = 2 * BLOCKIDNUM + 1;
    //剔除其中的索引块
    for (int i = 0; i < temp.size(); i++) {
        if ((i == id1) || (i == id2) || (i == id3) || ((i - id4) % (BLOCKIDNUM + 1) == 0)) {
            continue;
        }
        res.push_back(temp[i]);
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

void FileSystem::saveInodeInfo() {
    ofstream outfile;
    outfile.open("../records/usedIdode.txt", ios::out | ios::trunc);
    if (!outfile.is_open()) {
        cout << "文件打开失败!" << endl;
        return;
    }
    int inode_num = superBlock.getUsedInodeNum();
    outfile << inode_num << " ";
    for (int i = 0; i < DINODENUM; i++) {
        if (superBlock.isInodeUsed(i)) {
            //输出i结点信息
            outfile << i << " ";
            Dinode inode = diNode[i];
            auto rwct = inode.getRWCT();
            outfile << rwct.user_id << " ";
            int num = rwct.r_group.size();
            outfile << num << " ";
            for (int user_id:rwct.r_group) {
                outfile << user_id << " ";
            }
            num = rwct.w_group.size();
            outfile << num << " ";
            for (int user_id:rwct.w_group) {
                outfile << user_id << " ";
            }
            num = rwct.raw_group.size();
            outfile << num << " ";
            for (int user_id:rwct.raw_group) {
                outfile << user_id << " ";
            }
            num = rwct.null_group.size();
            outfile << num << " ";
            for (int user_id:rwct.null_group) {
                outfile << user_id << " ";
            }
            outfile << inode.getSize() << " ";
            outfile << inode.getDiskblockNum() << " ";
            outfile << inode.getDiskblockId() << " ";
            outfile << inode.getModifiedTime() << " ";
            outfile << inode.getCreatedTime() << " ";
        }
    }
    outfile.close();
}

void FileSystem::saveDiskInfo() {
    ofstream outfile;
    outfile.open("../records/usedDisk.txt", ios::out | ios::trunc);
    if (!outfile.is_open()) {
        cout << "文件打开失败!" << endl;
        return;
    }
    int disk_num = superBlock.getUsedDiskNum();
    outfile << disk_num << " ";
    for (int i = 0; i < DISKNUM; i++) {
        if (superBlock.isDiskUsed(i)) {
            outfile << i << " ";
            DiskBlock diskblock = diskBlock[i];
            int type = diskblock.getType();
            outfile << type << " ";
            int len = diskblock.getLen();
            outfile << len << " ";
            if (type == TEXTTYPE) {
                char content[BLOCKSIZE + 1];
                diskblock.getText(content);
                outfile << content << " ";
            } else {
                vector<int> indexList = diskblock.getIndexList();
                for (int index:indexList) {
                    outfile << index << " ";
                }
            }
        }
    }
    outfile.close();
}

void FileSystem::saveDirInfo() {
    ofstream outfile;
    outfile.open("../records/usedDir.txt", ios::out | ios::trunc);
    if (!outfile.is_open()) {
        cout << "文件打开失败!" << endl;
        return;
    }
    outfile << root_id << " ";
    int dir_num = superBlock.getUsedDirNum();
    outfile << dir_num << " ";
    for (int i = 0; i < DIRNUM; i++) {
        if (superBlock.isDirUsed(i)) {
            SFD cur_sfd = sfd[i];
            int num = cur_sfd.getNum();
            outfile << i << " ";
            outfile << num << " ";
            outfile << cur_sfd.toString();
        }
    }
    outfile.close();
}

bool FileSystem::readDiskInfo() {
    ifstream input;
    input.open("../records/userDisk.txt", ios::in);
    if (!input.is_open()) {
        return false;
    }
    int count;
    input >> count;
    while (count--) {
        int no;
        input >> no;
        auto diskblock = diskBlock[no];
        int type, len;
        input >> type;
        diskblock.setType(type);
        input >> len;
        diskblock.setLen(len);
        if (type == TEXTTYPE) {
            string content;
            input >> content;
            diskblock.writeText(content);
        } else {
            diskblock.clearIndex();
            int tmp;
            while (len--) {
                input >> tmp;
                diskblock.addIndex(tmp);
            }
        }
    }
    input.close();
    return true;
}

bool FileSystem::readInodeInfo() {
    ifstream input;
    input.open("../records/userInode.txt", ios::in);
    if (!input.is_open()) {
        return false;
    }
    int count;
    input >> count;
    while (count--) {
        int no;
        input >> no;
        auto node = diNode[no];
        RWCT rwct;
        input >> rwct.user_id;
        int size, tmp;
        input >> size;
        while (size--) {
            input >> tmp;
            rwct.r_group.push_back(tmp);
        }
        input >> size;
        while (size--) {
            input >> tmp;
            rwct.w_group.push_back(tmp);
        }
        input >> size;
        while (size--) {
            input >> tmp;
            rwct.raw_group.push_back(tmp);
        }
        input >> size;
        while (size--) {
            input >> tmp;
            rwct.null_group.push_back(tmp);
        }
        node.setRwct(rwct);
        input >> tmp;
        node.setSize(tmp);
        input >> tmp;
        node.setDiskBlockNum(tmp);
        input >> tmp;
        node.setDiskBlockId(tmp);
        time_t tmpTime;
        input >> tmpTime;
        node.setModifiedTime(tmpTime);
        input >> tmpTime;
        node.setCreatedTime(tmpTime);
    }
    input.close();
    return true;
}

bool FileSystem::readDirInfo() {
    ifstream input;
    input.open("../records/usedDir.txt", ios::in);
    if (!input.is_open()) {
        return false;
    }
    input >> root_id;
    int count;
    input >> count;
    while (count--) {
        int no, num;
        input >> no >> num;
        auto cur_sfd = sfd[no];
        int type, id;
        string name;
        while (num--) {
            input >> type >> name >> id;
            cur_sfd.addItem(SFD_ITEM(type, name, id));
        }
    }
    input.close();
    return true;
}

bool FileSystem::readFromFile() {
    return superBlock.readFromFile() & readInodeInfo() & readDiskInfo() & readDirInfo();
}

void FileSystem::saveToFile() {
    superBlock.saveTofile();
    saveInodeInfo();
    saveDiskInfo();
    saveDirInfo();
}

int FileSystem::getRootId() {
    return root_id;
}

SFD FileSystem::getSFD(int id) {
    return sfd[id];
}