#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <Category.h>
#include "texteditwindow.h"

const int DISK_SIZE = 1024;
const int BLOCK_SIZE = 4;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void UpdateTreeWidget();
    void UpdateTableWidget();
    void UpdateAddressLabel();

private:
    FCB* rootFCB; // 目录根结点的FCB
    Node* currentFolder; // 指向当前文件夹
    Node* rootFolder;
    QTreeWidgetItem* rootItem; // treeWidget控件的root
    Category* category;
    VirtualDisk* disk;

    QMenu* tableMenu;
    QMenu* tableItemMenu,* treeItemMenu;

    QAction* addNewFile,* addNewFolder;
    QAction* openF,* deleteF;
    QAction* format;

    QAction* openFolderInTree;
    QAction* addFolderInTree;

    void InitTreeWidget();
    void InitTableWidget();
    void _UpdateTreeWidget(Node* node, QTreeWidgetItem* item);

    void Create(Node* parentFolder, int type);
    void OpenEditor(QString fileName);

    Ui::MainWindow *ui;

private slots:
    void BackFolder(); // 返回上一级文件夹
    void SwitchFolder(Node* folder); // 跳转到某文件夹下

    void OpenTableMenu();
    void OpenTreeMenu();
    void CreateFile();
    void CreateFolder();
    void Format();
    void OpenItemInTable(int row);
    void DeleteItemInTable(int row);
    void OpenItemInTree(QTreeWidgetItem* item);
    void AddItemInTree(QTreeWidgetItem* item);

protected:
    void closeEvent(QCloseEvent *event);

};
#endif // MAINWINDOW_H
