#ifndef CATEGORY_H
#define CATEGORY_H

#include "VirtualDisk.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QStack>

class Node
{
public:
    FCB* fcb;
    Node* firstChild; // 左孩子
    Node* nextBrother; // 右兄弟
    Node* parent; // 父结点

    Node(FCB*);
    ~Node();
};

class Category
{
private:
    Node* root; // 目录根节点
    QStack<QJsonArray*> arrStack; // 日志文件控制
    QStack<QJsonObject*> objStack; // 日志文件控制
    VirtualDisk* disk; // 磁盘类
    void _WriteLog(Node* node, QJsonArray* brothers); // 写日志（递归）
    void _ReadLog(Node* parentNode, QJsonArray* sons); // 读日志（递归）
    void DeleteNode(Node* node); // 删除某单个结点即其在disk上的内容
    void FreeCategory(Node* node); // 删除某结点的所有子结点与右兄弟结点（及其子结点） （递归）

public:
    Category(Node* root, VirtualDisk* disk); // 初始化，需要传入根结点与disk类指针
    ~Category();
    Node* GetRootNode() { return root; }
    void Format(); // 格式化
    Node* Search(Node* parentFolder, QString name, int type); // 搜索结点
    void Create(Node* parentFolder, FCB* file); // 创建结点
    void Delete(Node* parentFolder, QString name, int type); // 删除结点
    bool UniqueName(Node* parentFolder, QString name, int type); // 判断文件名是否唯一
    bool ReadLog(); // 读日志
    bool WriteLog(); // 写日志
};

#endif // CATEGORY_H
