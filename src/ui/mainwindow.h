#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLogin();

    void mkdir();

    void copy();

    void cut();

    void touch();

    void paste();

    void open();

    void rename();

    void remove();

    void edit();

    void seeJurisdiction();

    void editJurisdiction();

    void on_logout_clicked();

    void on_listView_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_clicked();

signals:
    void logout();
    void quit();


private:
    Ui::MainWindow *ui;
    void updateView();
    QStandardItemModel *model = new QStandardItemModel();
    QMenu *emptyMenu;
    QMenu *itemMenu;
    bool isCopy;
    std::string from,to;
    QAction* pasteAct;
};

#endif // MAINWINDOW_H
