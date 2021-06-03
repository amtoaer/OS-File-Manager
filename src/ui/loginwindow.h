#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:

    void onLogout();

    void on_signIn_clicked();

    void on_signUp_clicked();

signals:
    void login();
    void quit();

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
