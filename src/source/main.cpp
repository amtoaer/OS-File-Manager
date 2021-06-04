//
// Created by amtoaer on 2021/5/27.
//
#include <QApplication>
#include <iostream>
#include "../header/view.h"
#include "../header/system.h"
#include "../ui/loginwindow.h"
#include "../ui/mainwindow.h"

using namespace std;


#include <QApplication>
View view;  //当前状态
UserManage user_mag;   //用户管理
FileSystem fs;  //文件系统


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow w;
    w.setWindowTitle("登录");
    MainWindow m;
    m.setWindowTitle("文件系统");
    w.show();
    QObject::connect(&w,SIGNAL(login()),&m,SLOT(onLogin()));
    QObject::connect(&m,SIGNAL(logout()),&w,SLOT(onLogout()));
    QObject::connect(&w,SIGNAL(quit()),&m,SLOT(quit()));
    QObject::connect(&m,SIGNAL(quit()),&w,SLOT(quit()));
    return a.exec();
}
