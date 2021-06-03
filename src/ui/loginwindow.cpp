#include <QMessageBox>

#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "../header/user.h"

extern UserManage user_mag;

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}


void LoginWindow::on_signIn_clicked()
{
    auto result = user_mag.login(ui->username->text().toStdString(),ui->password->text().toStdString());
    if (result){
        this->hide();
        emit login();
    }else{
        QMessageBox::information(this,"通知","登陆失败");
    }
}


void LoginWindow::on_signUp_clicked()
{
    auto result = user_mag.addUser(ui->username->text().toStdString(),ui->password->text().toStdString());
    if (result){
        QMessageBox::information(this,"通知","注册成功");
    }else{
        QMessageBox::information(this,"通知","注册失败");
    }
}

void LoginWindow::onLogout(){
    this->show();
}
