# File-System

## 运行

- 点击`File_System.exe`启动时，须保证`Category.json`与`Disk.json`文件**与`File_System.exe`在同一目录中**。
- 通过Qt Creator运行调试时，须保证`Category.json`与`Disk.json`文件在**生成的可执行文件的上一级目录中**（即与生成的`debug`或`release`文件夹在同一目录下）。
- 如果`Category.json`与`Disk.json`文件丢失，可从`src\log`文件夹中重新拷贝获得。
- 读写文件时尽量不要使用中文。

## 项目需求

### 基本任务

在内存中开辟一个空间作为文件存储器，在其上实现一个简单的文件系统。

退出这个文件系统时，需要该文件系统的内容保存到磁盘上，以便下次可以将其恢复到内存中来。

### 功能描述

- 文件存储空间管理可采取显式链接（如FAT）或者其他方法。（即自选一种方法）

- 空闲空间管理可采用位图或者其他方法。如果采用了位图，可将位图和FAT表合二为一。

- 文件目录采用多级目录结构。至于是否采用索引节点结构，自选。目录项目中应包含：文件名、物理地址、长度等信息。同学可在这里增加一些其他信息。

- 文件系统提供的操作：

  格式化，创建子目录，删除子目录，显示目录，更改当前目录，创建文件，打开文件，关闭文件，写文件，读文件，删除文件。

### 项目目的

- 熟悉文件存储空间的管理；
- 熟悉文件的物理结构、目录结构和文件操作；
- 熟悉文件系统管理实现；
- 加深对文件系统内部功能和实现过程的理解

## 开发环境

- **开发环境**：Windows10
- **开发软件**：
  1. Qt *v5.15.2*
  2. Qt Creator *v4.15.0* (Community)
- **开发语言**：C++
- **主要引用模块**：
  1. Qt核心组件: QObject, QMainWindow, QCoreApplication,
  2. QtWidgets: QMessageBox, QInputDialog, QLabel, QPushButton, QTreeWidgetItem
  3. Qt标准库: QStack, QString
  4. JSON相关: QJsonArray, QJsonObject, QJsonDocument, QJsonValue, QJsonParseError
  5. 其他: QCloseEvent, QDateTime, QDir, QFile

## 项目结构

```
D:.
│  README.md
├─images ——README图片
│      addNewFile.png
│      deleteFolder.png
│      format.png
│      MainWindow.png
│      OpneFile.png
│      sameFileName.png
│      SaveFile.png
│      saveLog.png
│      switchFile.png
│      WhiteSpace.png
│
└─src
    │  Category.cpp ——Category目录类实现
    │  Category.h
    │  FCB.cpp ——FCB类实现
    │  FCB.h
    │  FileManagement.pro ——Qt项目文件
    │  FileManagement.pro.user
    │  images.qrc ——Qt资源管理文件
    │  main.cpp
    │  mainwindow.cpp ——主界面逻辑实现
    │  mainwindow.h
    │  mainwindow.ui ——主界面UI
    │  texteditwindow.cpp ——编辑器界面逻辑实现
    │  texteditwindow.h
    │  texteditwindow.ui ——编辑器界面UI
    │  VirtualDisk.cpp ——VirtualDisk模拟磁盘类实现
    │  VirtualDisk.h
    │
    ├─images ——项目图片资源文件
    │      editor.ico
    │      file.ico
    │      folder.ico
    │      txt.ico
    │      user.ico
    │
    └─log ——日志文件
            Category.json
            Disk.json
```

## 操作说明

### 主界面

<img src = "images\MainWindow.png" width = 600>

### 打开文件

- 在右侧文件列表中，双击某文件。
- 右键选中某文件，弹出菜单，并选择`Open`。

<img src = "images\OpneFile.png" width = 600>

### 保存已编辑文件

点击文件编辑页面右上角"X"，并选择是否`Save file`。

<img src="images\SaveFile.png" width=600>

### 新建文件或文件夹

- 在右侧文件列表空白处（如下图红框所示），鼠标右键打开菜单，并选择`Add new file`或`Add new folder`。
- 在左侧文件目录树中，选中某级文件夹后鼠标右键打开菜单，选择`Add new folder`（此方式仅能新建文件夹）。

<img src="images\WhiteSpace.png" width=600>

**注意：同一目录下，文件夹或文件不能重名！**

<img src="images\addNewFile.png" width=600>

<img src = "images\sameFileName.png" width = 600>

### 打开文件夹

- 在右侧文件列表中，双击某文件夹，实现跳转。
- 在左侧文件目录树中，双击某文件夹，实现跳转。
- 在左侧文件目录树中，左键选中某文件夹，弹出菜单，选择`Open`。
- 右上方点击`Back`按钮，可实现跳转至上一级文件夹。

<img src = "images\switchFile.png" width = 600>

### 删除文件或文件夹

在右侧文件目录列表中右键某文件或文件夹，弹出菜单，选择`Delete`。

<img src = "images\deleteFolder.png" width = 600>

### 格式化

在右侧文件目录列表空白处右键，弹出菜单，选择`Format`。

<img src = "images\format.png" width = 600>

### 保存到日志文件

点击右上角主界面的"X"，弹出对话框，选择`Yes`。

<img src = "images\saveLog.png" width = 600>

## 待优化

1. 编码：支持中文。
2. 多线程：可打开多个编辑器窗口。
3. 布局：支持动态大小的窗口布局。
4. 文件或文件夹重名功能。
