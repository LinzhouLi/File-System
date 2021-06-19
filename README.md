# File-System

## 运行

- 点击`File_System.exe`启动时，须保证`log`文件夹**与`File_System.exe`在同一目录中**。
- 通过Qt Creator运行调试时，须保证`log`文件夹在**生成的可执行文件的上一级目录中**（即与生成的`debug`或`release`文件夹在同一目录下）。
- 如果`log`文件夹丢失，可从`src`文件夹中重新拷贝获得。

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
    ├─images ——图片资源文件
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

