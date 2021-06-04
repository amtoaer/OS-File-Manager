#include <QStandardItem>
#include <QInputDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../header/view.h"
#include "../header/system.h"
#include "../header/utils.h"

extern View view;
extern FileSystem fs;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    emptyMenu = new QMenu(ui->listView);
    itemMenu = new QMenu(ui->listView);
    auto mkdir = emptyMenu->addAction("新建文件夹");
    auto touch = emptyMenu->addAction("新建文件");
    pasteAct = emptyMenu->addAction("粘贴");
    pasteAct->setEnabled(false);
    auto open = itemMenu->addAction("查看");
    auto edit = itemMenu->addAction("编辑");
    auto copy = itemMenu->addAction("复制");
    auto cut = itemMenu->addAction("剪切");
    auto rename = itemMenu->addAction("重命名");
    auto remove = itemMenu->addAction("删除");
    auto seeJurisdiction = itemMenu->addAction("查看权限");
    auto editJurisdiction = itemMenu->addAction("修改权限");
    connect(mkdir,SIGNAL(triggered(bool)),this,SLOT(mkdir()));
    connect(touch,SIGNAL(triggered(bool)),this,SLOT(touch()));
    connect(pasteAct,SIGNAL(triggered(bool)),this,SLOT(paste()));
    connect(open,SIGNAL(triggered(bool)),this,SLOT(open()));
    connect(copy,SIGNAL(triggered(bool)),this,SLOT(copy()));
    connect(cut,SIGNAL(triggered(bool)),this,SLOT(cut()));
    connect(rename,SIGNAL(triggered(bool)),this,SLOT(rename()));
    connect(remove,SIGNAL(triggered(bool)),this,SLOT(remove()));
    connect(edit,SIGNAL(triggered(bool)),this,SLOT(edit()));
    connect(seeJurisdiction,SIGNAL(triggered(bool)),this,SLOT(seeJurisdiction()));
    connect(editJurisdiction,SIGNAL(triggered(bool)),this,SLOT(editJurisdiction()));
    ui->listView->setModel(model);
    ui->listView->setWrapping(true);
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView->setFlow(QListView::LeftToRight);
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listView->setIconSize(QSize(64, 64));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete itemMenu;
    delete emptyMenu;
}

void MainWindow::updateView(){
    ui->currentPath->setText(QString::fromStdString(view.cur_path));
    if (view.cur_path=="/"){
        ui->pushButton->setEnabled(false);
    }else{
        ui->pushButton->setEnabled(true);
    }
    model->clear();
    auto loc = fs.findDir(view.cur_path);
    auto content = fs.getSFD(loc).getAllNext();
    for (int i=0;i<content.size();i++){
        auto item = content.at(i);
        auto iconPath = item.type==1?"./file.png":"./dir.png";
        auto type = item.type==1?"file":"dir";
        auto qitem = new QStandardItem(QIcon(iconPath),QString::fromStdString(item.name));
        qitem->setWhatsThis(type);
        model->appendRow(qitem);
    }
}

void MainWindow::onLogin(){

    ui->welcomeLabel->setText("欢迎您,"+QString::fromStdString(view.cur_user.getUsername()));
    this->updateView();
    this->show();
}

void MainWindow::on_logout_clicked()
{
    this->hide();
    emit logout();
}

void MainWindow::mkdir(){
    bool ok=false;
    auto text = QInputDialog::getText(this,"提示","请输入文件夹名：",QLineEdit::Normal,"",&ok);
    if (ok && !text.isEmpty()){
        fs.mkdir(next(text.toStdString()));
    }
    updateView();
}

void MainWindow::copy(){
    auto name = model->itemFromIndex(ui->listView->selectionModel()->selectedIndexes().at(0))->text();
    from = next(name.toStdString());
    isCopy = true;
    pasteAct->setEnabled(true);
}

void MainWindow::cut(){
    auto name = model->itemFromIndex(ui->listView->selectionModel()->selectedIndexes().at(0))->text();
    from = next(name.toStdString());
    isCopy = false;
    pasteAct->setEnabled(true);
}

void MainWindow::paste(){
    to = view.cur_path;
    if (isCopy){
        auto success = fs.cp(from,to);
        if (!success){
            QMessageBox::critical(this,"错误","复制失败");
        }
    }else{
        auto success = fs.mv(from,to);
        if (!success){
            QMessageBox::critical(this,"错误","移动失败");
        }
    }
    pasteAct->setEnabled(false);
    updateView();
}

void MainWindow::touch(){
    bool ok=false;
    auto text = QInputDialog::getText(this,"提示","请输入文件名：",QLineEdit::Normal,"",&ok);
    if (ok && !text.isEmpty()){
        fs.touch(next(text.toStdString()));
    }
    updateView();
}

void MainWindow::open(){
    auto item = model->itemFromIndex(ui->listView->selectionModel()->selectedIndexes().at(0));
    auto type = item->whatsThis();
    auto name = item->text();
    if (type=="dir"){
        fs.cd(next(name.toStdString()));
        updateView();
    }else{
        auto content =fs.readFile(next(name.toStdString()));
        QMessageBox::information(this,"文件内容",QString::fromStdString(content));
    }
}

void MainWindow::edit(){
    auto item = model->itemFromIndex(ui->listView->selectionModel()->selectedIndexes().at(0));
    auto type = item->whatsThis();
    auto name = item->text();
    if (type=="file"){
        auto content  = fs.readFile(next(name.toStdString()));
        bool ok=false;
        auto text = QInputDialog::getText(this,name,"文件内容",QLineEdit::Normal,QString::fromStdString(content),&ok);
        if (ok && !text.isEmpty()){
            bool result = fs.writeFile(next(name.toStdString()),text.toStdString());
            if (!result){
                QMessageBox::critical(this,"错误","保存失败");
            }
        }
    }else{
        QMessageBox::critical(this,"错误","只有文件支持编辑");
    }
}

void MainWindow::rename(){
    auto name = model->itemFromIndex(ui->listView->selectionModel()->selectedIndexes().at(0))->text();
    bool ok=false;
    auto text = QInputDialog::getText(this,"提示","请输入新名称：",QLineEdit::Normal,"",&ok);
    if (ok && !text.isEmpty()){
        fs.rename(next(name.toStdString()),text.toStdString());
    }
    updateView();
}

void MainWindow::remove(){
    auto name = model->itemFromIndex(ui->listView->selectionModel()->selectedIndexes().at(0))->text();
    fs.rm(next(name.toStdString()));
    updateView();
}

void MainWindow::seeJurisdiction(){
    auto item = model->itemFromIndex(ui->listView->selectionModel()->selectedIndexes().at(0));
    auto name = item->text();
    auto type = item->whatsThis();
    if (type=="dir"){
        QMessageBox::critical(this,"错误","只有文件支持权限查看");
        return;
    }
    auto path = next(name.toStdString());
    auto rwct = fs.getRwct(fs.findFile(path));
    auto r= QStringList();
    auto w = QStringList();
    auto rw = QStringList();
    auto no = QStringList();
    for (auto item:rwct.null_group){
        no.append(QString::fromStdString(user_mag.getName(item)));
    }
    for (auto item: rwct.r_group){
        r.append(QString::fromStdString(user_mag.getName(item)));
    }
    for (auto item:rwct.raw_group){
        rw.append(QString::fromStdString(user_mag.getName(item)));
    }
    for (auto item:rwct.w_group){
        w.append(QString::fromStdString(user_mag.getName(item)));
    }
    auto message = "读权限列表："+r.join("、")+"\n"+"写权限列表："+w.join("、")+"\n"+"读写权限列表："+rw.join("、")+"\n"+"无权限列表："+no.join("、");
    QMessageBox::information(this,"提示",message);
}

void MainWindow::editJurisdiction(){
    auto item = model->itemFromIndex(ui->listView->selectionModel()->selectedIndexes().at(0));
    auto name = item->text();
    auto type = item->whatsThis();
    if (type=="dir"){
        QMessageBox::critical(this,"错误","只有文件支持权限编辑");
        return;
    }
    auto path =next(name.toStdString());
    bool ok = false;
    auto list = QStringList();
    for (auto item:user_mag.getUserName()){
        list.append(QString::fromStdString(item));
    }
    auto text = QInputDialog::getText(this,"提示","请输入要修改权限的用户名：",QLineEdit::Normal,list.join("、"),&ok);
    if (ok&&!text.isEmpty()){
        auto jurisdiction = QInputDialog::getText(this,"提示","请输入要修改为的权限：",QLineEdit::Normal,"or、ow、raw、no",&ok);
        if (ok&&!jurisdiction.isEmpty()){
            auto result = fs.changeFileRWCT(path,text.toStdString(),jurisdiction.toStdString());
            if (!result){
                QMessageBox::critical(this,"错误","修改权限失败，请确保您拥有此文件");
            }
        }
    }
}

void MainWindow::on_listView_customContextMenuRequested(const QPoint &pos)
{
    auto selectItem = ui->listView->selectionModel()->selectedIndexes().empty();
    if (selectItem){
        emptyMenu->exec(QCursor::pos());
    }else{
        itemMenu->exec(QCursor::pos());
    }
}


void MainWindow::on_pushButton_clicked()
{
    fs.goBack();
    updateView();
}


void MainWindow::on_save_clicked()
{
    fs.saveToFile();
    user_mag.saveToFile();
    QMessageBox::information(this,"提示","保存完成");
}


void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    auto item = model->itemFromIndex(index);
    auto type = item->whatsThis();
    auto name = item->text();
    if (type=="dir"){
        fs.cd(next(name.toStdString()));
        updateView();
    }else{
        auto content =fs.readFile(next(name.toStdString()));
        QMessageBox::information(this,"文件内容",QString::fromStdString(content));
    }
}

