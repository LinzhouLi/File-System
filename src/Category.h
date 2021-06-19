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
    Node* root;
    QStack<QJsonArray*> arrStack;
    QStack<QJsonObject*> objStack;
    VirtualDisk* disk;
    void _WriteLog(Node* node, QJsonArray* brothers);
    void _ReadLog(Node* parentNode, QJsonArray* sons);

public:
    Category(Node* root, VirtualDisk* disk);
    ~Category();
    Node* GetRootNode() { return root; }
    void FreeCategory(Node* node);
    void DeleteNode(Node* node);
    void Format();
    Node* Search(Node* parentFolder, QString name, int type);
    void Create(Node* parentFolder, FCB* file);
    void Delete(Node* parentFolder, QString name, int type);
    //void DeleteFile(Node* parentFolder, QString name);
    bool UniqueName(Node* parentFolder, QString name, int type);
    bool ReadLog();
    bool WriteLog();
};

#endif // CATEGORY_H
