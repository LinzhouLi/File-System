#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPalette>
#include <QStack>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->addressLabel->setStyleSheet("QLabel{background:#ffffff;}");
    this->setWindowTitle("File Management System");
    this->setWindowIcon(QIcon(":/images/file.ico"));

    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton); // 去掉对话框右上角的"?"

    // 设置category目录
    disk = new VirtualDisk(DISK_SIZE, BLOCK_SIZE);
    rootFCB = new FCB(QDir::home().dirName(), FOLDER); // 获取本机用户名
    rootFolder = new Node(rootFCB);
    category = new Category(rootFolder, disk);
    //从JSON中读取category,disk
    if(!(category->ReadLog() && disk->ReadLog()))
        QMessageBox::critical(this, "Error", "Fail to initialize from log file!");

    InitTreeWidget();
    InitTableWidget();
    UpdateAddressLabel();

    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::BackFolder);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete rootItem;
    delete tableMenu;
    delete tableItemMenu;
    delete treeItemMenu;
    delete addNewFile;
    delete addNewFolder;
    delete openF;
    delete deleteF;
    delete format;
    delete openFolderInTree;
    delete addFolderInTree;

    delete category; // 注意析构顺序
    delete disk;
    delete rootFolder;
}

void MainWindow::InitTreeWidget()
{
    // 设置tree控件
    rootItem = nullptr;
    ui->treeWidget->setHeaderLabel("File Category");
    // 字体加粗
    QFont font = ui->treeWidget->header()->font();
    font.setBold(true);
    ui->treeWidget->header()->setFont(font);

    // 设置menu菜单
    openFolderInTree = new QAction("Open", this);
    addFolderInTree = new QAction("Add new folder", this);
    treeItemMenu = new QMenu(this);
    treeItemMenu->addAction(openFolderInTree);
    treeItemMenu->addAction(addFolderInTree);
    // 设置menu菜单弹出的函数
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::OpenTreeMenu);
    // 设置menu菜单槽函数
    connect(openFolderInTree, &QAction::triggered, this, [=](){
        auto items = ui->treeWidget->selectedItems();
        OpenItemInTree(items.at(0));
    });
    connect(addFolderInTree, &QAction::triggered, this, [=](){
        auto items = ui->treeWidget->selectedItems();
        AddItemInTree(items.at(0));
    });

    // 响应双击,打开文件夹
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(OpenItemInTree(QTreeWidgetItem*)));

    // 初始化tree目录
    UpdateTreeWidget();
}

void MainWindow::InitTableWidget()
{
    // 设置表头
    QStringList header;
    ui->tableWidget->setColumnCount(4);
    header << tr("File Name") << tr("Last Modified") << tr("Type")<<tr("Size");
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->verticalHeader()->setVisible(false); // 不显示列表头
    //设置每列宽度
    ui->tableWidget->setColumnWidth(0,250);
    ui->tableWidget->setColumnWidth(1,270);
    ui->tableWidget->setColumnWidth(2,150);
    ui->tableWidget->setColumnWidth(3,129);
    // 不显示格线
    ui->tableWidget->setShowGrid(false);
    // 禁止修改
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置为仅选中单行
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QTableWidget::SingleSelection);
    // 字体加粗
    QFont font = ui->tableWidget->horizontalHeader()->font();
    font.setBold(true);
    ui->tableWidget->horizontalHeader()->setFont(font);

    // 设置menu菜单
    tableMenu = new QMenu(this);
    addNewFile = new QAction("Add new file", this);
    addNewFolder = new QAction("Add new folder", this);
    format = new QAction("Format", this);
    // 绑定menu菜单与添加文件，添加文件夹，格式化操作
    tableMenu->addAction(addNewFile);
    tableMenu->addAction(addNewFolder);
    tableMenu->addAction(format);

    // 设置item menu菜单
    tableItemMenu = new QMenu(this);
    openF = new QAction("Open", this);
    deleteF = new QAction("Delete", this);
    // 绑定item menu菜单与打开，删除
    tableItemMenu->addAction(openF);
    tableItemMenu->addAction(deleteF);

    // 设置menu菜单弹出的函数
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::OpenTableMenu);

    // 设置添加文件，添加文件夹，格式化操作的槽函数
    connect(addNewFile, &QAction::triggered, this, &MainWindow::CreateFile);
    connect(addNewFolder, &QAction::triggered, this, &MainWindow::CreateFolder);
    connect(format, &QAction::triggered, this, &MainWindow::Format);
    // 设置打开文件槽函数
    connect(openF, &QAction::triggered, this, [=](){//
        auto items = ui->tableWidget->selectedItems();
        OpenItemInTable(items.at(0)->row()); // 打开文件相当于双击
    });
    //设置删除文件槽函数
    connect(deleteF, &QAction::triggered, this, [=](){
        auto items = ui->tableWidget->selectedItems();
        DeleteItemInTable(items.at(0)->row()); // 打开文件相当于双击
    });

    //初始化根目录页面
    currentFolder = category->GetRootNode();
    UpdateTableWidget();

    // 响应双击,打开文件或目录
    connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(OpenItemInTable(int)));//
}

void MainWindow::BackFolder()
{
    if(currentFolder == rootFolder)
        return;
    else
        SwitchFolder(currentFolder->parent);
}

void MainWindow::SwitchFolder(Node* folder)
{
    currentFolder = folder;
    UpdateTableWidget();
    UpdateAddressLabel();
}

void MainWindow::UpdateTreeWidget()
{
    if(rootItem != nullptr)
    {
        delete rootItem;
        rootItem = nullptr;
    }

    rootItem = new QTreeWidgetItem(ui->treeWidget);
    rootItem->setText(0, rootFCB->fileName);
    rootItem->setIcon(0, QIcon(":/images/user.ico"));
    ui->treeWidget->addTopLevelItem(rootItem);

    _UpdateTreeWidget(rootFolder->firstChild, rootItem);
    ui->treeWidget->expandAll(); // 展开树
}

void MainWindow::_UpdateTreeWidget(Node* node, QTreeWidgetItem* parentItem)
{
    if(node == nullptr) return;
    if(node->fcb->type == TXTFILE)
        _UpdateTreeWidget(node->nextBrother, parentItem); // 向兄弟方向递归
    if(node->fcb->type == FOLDER)
    {
        //添加文件夹到控tree件
        QTreeWidgetItem* sonItem = new QTreeWidgetItem(parentItem);
        sonItem->setText(0, node->fcb->fileName);
        sonItem->setIcon(0, QIcon(":/images/folder.ico"));

        _UpdateTreeWidget(node->firstChild, sonItem); // 向子树方向递归
        _UpdateTreeWidget(node->nextBrother,parentItem); // 向兄弟方向递归
    }
}

void MainWindow::UpdateAddressLabel()
{
    QString address = "";
    Node* temp = currentFolder;
    while(temp != nullptr)
    {
        address = temp->fcb->fileName + ">" +address;
        temp = temp->parent;
    }
    ui->addressLabel->setText(" " + address);
}

void MainWindow::UpdateTableWidget()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    Node* temp = currentFolder->firstChild;
    int rowCount;
    while(temp != nullptr)
    {
        // 添加一行
        rowCount = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rowCount);

        // 添加元素
        if(temp->fcb->type == TXTFILE) // txt
        {
            ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QIcon(":/images/txt.ico"), temp->fcb->fileName));
            ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem("TXT File"));
            ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(temp->fcb->size) + "B"));
        }
        else // folder
        {
            ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QIcon(":/images/folder.ico"), temp->fcb->fileName));
            ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem("Folder"));
            ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(""));
        }
        ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(temp->fcb->lastModify));

        // 菜单

        temp = temp->nextBrother;
    }
}

void MainWindow::OpenTableMenu()
{
    QPoint pos = QCursor::pos(); // 鼠标位置
    QPoint posInTable = ui->tableWidget->mapFromGlobal(pos); // 鼠标位置对应的table widget中的坐标
    posInTable.ry() -= ui->tableWidget->horizontalHeader()->height(); // 纵坐标加上表头高度，很坑！！！！！
    QTableWidgetItem* item = ui->tableWidget->itemAt(posInTable); // 用此坐标找到对应item
    if(item != nullptr) // 若此item存在，即鼠标在某行上
        tableItemMenu->exec(pos); // 则打开item menu菜单
    else
        tableMenu->exec(pos); // 否则打开menu菜单
}

void MainWindow::OpenTreeMenu()
{
    QPoint pos = QCursor::pos(); // 鼠标位置
    QPoint posInTree = ui->treeWidget->mapFromGlobal(pos); // 鼠标位置对应的table widget中的坐标
    posInTree.ry() -= ui->treeWidget->header()->height(); // 纵坐标加上表头高度，很坑！！！！！
    QTreeWidgetItem* item = ui->treeWidget->itemAt(posInTree); // 用此坐标找到对应item
    if(item != nullptr) // 若此item存在，即鼠标在某行上
        treeItemMenu->exec(pos); // 则打开item menu菜单
}

void MainWindow::CreateFile()
{
    Create(currentFolder, TXTFILE);
    UpdateTableWidget();
}

void MainWindow::OpenItemInTable(int row)
{
    QString fileName = ui->tableWidget->item(row, 0)->text();
    QString fileType = ui->tableWidget->item(row, 2)->text();

    if(fileType == "TXT File") // 如果是TXT文件，双击进入编辑器
    {
        Node* file;
        file = category->Search(currentFolder, fileName, TXTFILE);

        QString oldtext = disk->GetFileContent(file->fcb); // 显示文件原始信息
        QString newtext = oldtext;
        TextEditWindow editor(this, fileName, &newtext); // 新建文件编辑器窗口
        editor.show();

        // 等待editor窗口关闭
        QEventLoop loop;
        connect(&editor, SIGNAL(QuitEditor()), &loop, SLOT(quit()));
        loop.exec();

        if(oldtext != newtext) // 如果文件有变化
        {
            if(!disk->UpdateFileContent(file->fcb, newtext)) // 分配磁盘空间失败
                QMessageBox::critical(this, "Error", "Out of memory!");
            UpdateTableWidget();
        }
    }
    else // 如果是文件夹，双击进入此文件夹
    {
        Node* folder;
        folder = category->Search(currentFolder, fileName, FOLDER);
        SwitchFolder(folder);
    }
}

void MainWindow::DeleteItemInTable(int row)
{
    QString fileName = ui->tableWidget->item(row, 0)->text();
    QString fileType = ui->tableWidget->item(row, 2)->text();

    if(fileType == "TXT File") // 如果是TXT文件
    {
        auto temp = QMessageBox::warning(this, "Warning", "Permanently delete this file?", QMessageBox::Yes | QMessageBox::No);
        if (temp == QMessageBox::Yes) // 确认删除
        {
            category->Delete(currentFolder, fileName, TXTFILE);
            UpdateTableWidget(); // 更新
        }
    }
    else // 如果是文件夹
    {
        auto temp = QMessageBox::warning(this, "Warning", "Permanently delete this folder?", QMessageBox::Yes | QMessageBox::No);
        if (temp == QMessageBox::Yes) // 确认删除
        {
            category->Delete(currentFolder, fileName, FOLDER);
            UpdateTreeWidget();
            UpdateTableWidget(); // 更新
        }
    }
}

void MainWindow::OpenItemInTree(QTreeWidgetItem* item)
{
    QStack<QString> stack;

    //把路径信息保存在stack中
    while(item != rootItem)
    {
        stack.push(item->text(0));
        item = item->parent();
    }

    // 依次从目录树中找到路径信息
    Node* temp = rootFolder;
    while(!stack.isEmpty())
        temp = category->Search(temp, stack.pop(), FOLDER);

    SwitchFolder(temp);
}

void MainWindow::AddItemInTree(QTreeWidgetItem* item)
{
    QStack<QString> stack;

    //此结点为根结点
    if(item == rootItem)
    {
        Create(rootFolder, FOLDER);
        return;
    }

    //把路径信息保存在stack中
    while(item != rootItem)
    {
        stack.push(item->text(0));
        item = item->parent();
    }

    // 从目录树中找到父结点
    Node* parentFolder = rootFolder;
    while(stack.length() != 1)
        parentFolder = category->Search(parentFolder, stack.pop(), FOLDER);

    //添加文件夹
    Create(parentFolder, FOLDER);
}

void MainWindow::CreateFolder()
{
    Create(currentFolder, FOLDER);
    UpdateTableWidget();
}

void MainWindow::Create(Node *parentFolder, int type)
{
    bool ok;
    QString dialogText = type == FOLDER ? "Folder" : "File";
    QString name = QInputDialog::getText(this, "Input", dialogText + " name:",
                                         QLineEdit::Normal, "New " + dialogText, &ok);

    if(!ok)return;
    if(name.isEmpty()) // 文件名不能为空
    {
        QMessageBox::critical(this, "Error", dialogText + " name can NOT be empty!");
        return;
    }
    if(name[0] == rootFCB->fileName) // 文件名不能以"_"开头
    {
        QMessageBox::critical(this, "Error", dialogText + " name can NOT be same with the root folder!");
        return;
    }
    if(!category->UniqueName(parentFolder, name, type)) // 文件不能重名
    {
        QMessageBox::critical(this, "Error", dialogText + " name must be unique!");
        return;
    }

    FCB* fcb;
    if(type == FOLDER) // 新建文件夹
    {
        fcb = new FCB(name, FOLDER);
        category->Create(parentFolder, fcb);
        UpdateTreeWidget();
    }
    else // 新建TXT文件
    {

        QString text = "";
        TextEditWindow editor(this, name, &text); // 新建文件编辑器窗口
        editor.show();

        // 等待editor窗口关闭
        QEventLoop loop;
        connect(&editor, SIGNAL(QuitEditor()), &loop, SLOT(quit()));
        loop.exec();

        fcb = new FCB(name, TXTFILE);
        if(!disk->AllocMem(fcb, text)) // 分配磁盘空间失败
        {
            QMessageBox::critical(this, "Error", "Out of memory!");
            delete fcb;
        }
        else // 成功新建TXT
            category->Create(parentFolder, fcb);
    }
}

void MainWindow::Format()
{
    auto temp = QMessageBox::warning(this, "Warning", "Permanently delete ALL files?", QMessageBox::Yes | QMessageBox::No);
    if (temp == QMessageBox::Yes) // 确认删除
    {
        category->Format();
        disk->Format();
        rootFCB->SetTime();
        currentFolder = rootFolder;
        UpdateTreeWidget();
        UpdateTableWidget(); // 更新
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    auto temp = QMessageBox::information(this, "Save", "Save the log file?", QMessageBox::Yes | QMessageBox::No);
    if (temp == QMessageBox::Yes) // 确认保存
    {
        if(!(category->WriteLog() && disk->WriteLog())) // 注意两个都为真的写法，坑！
                QMessageBox::critical(this, "Error", "Fail to save the log file!");
    }
    event->accept();
    return;
}
