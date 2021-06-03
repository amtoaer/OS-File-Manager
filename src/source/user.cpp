//
// Created by lenovo on 2021-05-29.
//

#include "../header/user.h"
#include "../header/view.h"
#include "../header/system.h"


vector<int> UserManage::getUserId() {
    vector<int> user_id;
    for (int i = 0; i < USERNUM; i++) {
        if (users[i].getId() != -1) {
            user_id.push_back(users[i].getId());
        }
    }
    return user_id;
}

UserManage::UserManage() {
    // 尝试读取保存的文件
    this->readFromFile();
}

bool UserManage::addUser(string username, string userpwd) {//注册一个用户
    for (int i = 0; i < USERNUM; i++) {
        if (users[i].getId() == -1) {
            users[i].setId(i);
            users[i].setUsername(username);
            users[i].setUserpwd(userpwd);
            user_num++;
            return true;//注册成功
        }
    }
    return false;//用户已满，注册失败
}

bool UserManage::deleteUser(string username, string userpwd) {//注销一个用户
    for (auto &user : users) {
        if (user.isUsername(username)) {
            if (!user.isUserpwd(userpwd)) {
                return false;//密码错误
            }
            user.setId(-1);
            user.setUsername("");
            user.setUserpwd("");
            user_num--;
            return true;//注销成功
        }
    }
    return false;//未在用户信息中查到此人
}

bool UserManage::login(string username, string userpwd) {//用户登陆
    for (User user : users) {
        if (user.isUsername(username)) {
            if (user.isUserpwd(userpwd)) {
                //设置当前视图
                view.cur_user = user;
                view.cur_path = "/";
                view.cur_sfd_id = fs.getRootId();
                return true;//登陆成功，用户为user
            }
            return false;//密码错误
        }
    }
    return false;//未在用户信息中查到此人
}

int UserManage::getId(string name) { //通过用户名得到用户id
    for (User user: users) {
        if (user.isUsername(name)) {
            return user.getId();
        }
    }
    return -1;
}

void UserManage::saveToFile() {
    ofstream outfile;
    outfile.open("../records/user.txt", ios::out | ios::trunc);
    if (!outfile.is_open()) {
        cout << "文件打开失败!" << endl;
        return;
    }
    outfile << user_num << " ";
    for (int i = 0; i < USERNUM; i++) {
        User user = users[i];
        if (user.getId() != -1) {
            outfile << user.getId() << " " << user.getUsername() << " " << user.getPassword() << " ";
        }
    }
    outfile.close();
}

void UserManage::readFromFile() {
    ifstream input;
    input.open("../records/user.txt", ios::in);
    if (!input.is_open()) {
        return;
    }
    int id;
    string username, password;
    while (input >> id >> username >> password) {
        users[id].setId(id);
        users[id].setUsername(username);
        users[id].setUserpwd(password);
    }
    input.close();
}