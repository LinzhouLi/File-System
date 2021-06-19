#include "Category.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QTextStream>
#include <QFile>

Node::Node(FCB* fcb)
{
    this->fcb = fcb;
    this->firstChild = nullptr;
    this->nextBrother = nullptr;
    this->parent = nullptr;
}

Node::~Node()
{
    delete fcb;
}

Category::Category(Node* root, VirtualDisk* disk)
{
    //if(root == nullptr)
    this->root = root;
    this->disk = disk;
}

Category::~Category()
{
    int i;
    for(i = 0; i < arrStack.size(); i++)
        delete arrStack[i];
    for(i = 0; i < objStack.size(); i++)
        delete objStack[i];
    Format();
}

void Category::FreeCategory(Node* node)
{
    if(node == nullptr) return;

    if(node->firstChild != nullptr)
    {
        FreeCategory(node->firstChild);
        node->firstChild = nullptr;
    }
    if(node->nextBrother != nullptr)
    {
        FreeCategory(node->nextBrother);
        node->nextBrother = nullptr;
    }

    DeleteNode(node);
}

void Category::DeleteNode(Node* node)
{
    disk->DeleteFileContent(node->fcb);//同步删除磁盘文件
    delete node;
}

Node* Category::Search(Node* parentFolder, QString name, int type)
{
    Node* node = parentFolder->firstChild;

    while(node != nullptr)
    {
        if(node->fcb->fileName == name && node->fcb->type == type)
            return node;
        node = node->nextBrother;
    }
    return nullptr; // 没找到
}

void Category::Create(Node* parentFolder, FCB* file)
{
    if(root == nullptr || parentFolder == nullptr) return;

    if(parentFolder->firstChild == nullptr) // 如果是父文件夹下第一个文件
    {
        parentFolder->firstChild = new Node(file);
        parentFolder->firstChild->parent = parentFolder;
    }
    else // 如果不是父文件夹下第一个文件
    {
        Node* temp = parentFolder->firstChild;
        while(temp->nextBrother != nullptr) // 找到最后一个文件
            temp = temp->nextBrother;

        temp->nextBrother = new Node(file);
        temp->nextBrother->parent = parentFolder;
    }
}

void Category::Delete(Node* parentFolder, QString name, int type)
{
    Node* thisF = Search(parentFolder, name, type);

    if(parentFolder->firstChild == thisF)// 如果是父文件夹下第一个文件
        parentFolder->firstChild = thisF->nextBrother;
    else// 如果不是父文件夹下第一个文件
    {
        Node* temp = parentFolder->firstChild;
        while(temp->nextBrother != thisF)
            temp = temp->nextBrother;

        temp->nextBrother = thisF->nextBrother;
    }

    if(type == FOLDER) FreeCategory(thisF->firstChild);
    DeleteNode(thisF);
}
/*
void Category::DeleteFile(Node* parentFolder, QString name)
{
    Node* thisFile = Search(parentFolder, name, TXTFILE);

    if(parentFolder->firstChild == thisFile)// 如果是父文件夹下第一个文件
        parentFolder->firstChild = thisFile->nextBrother;
    else// 如果不是父文件夹下第一个文件
    {
        Node* temp = parentFolder->firstChild;
        while(temp->nextBrother != thisFile)
            temp = temp->nextBrother;

        temp->nextBrother = thisFile;
    }

    DeleteNode(thisFile);
}*/

bool Category::UniqueName(Node* parentFolder, QString name, int type)
{
    Node* node = parentFolder->firstChild;

    // 目录下无文件
    if(node == nullptr) return true;

    while(node != nullptr)
    {
        if(node->fcb->fileName == name && node->fcb->type == type)
            return false;
        node = node->nextBrother;
    }

    return true;
}

void Category::Format()
{
    FreeCategory(root->firstChild);
    root->firstChild = nullptr;
}

bool Category::ReadLog()
{
    // 读Json文件
    QFile file("log/CategoryLog.json");
    if(!file.open(QIODevice::Text | QIODevice::ReadOnly)) // 文件读取错误
        return false;
    QJsonParseError err; // 错误标识
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if(err.error != QJsonParseError::NoError) // JSON格式错误
        return false;

    // 判断category是否为空
    QJsonObject obj = doc.object();
    QJsonValue category = obj.value("Category");
    if(category.type() == QJsonValue::Null) // 空category
        return true;

    // 生成category
    QJsonArray* sons = new QJsonArray(category.toArray());
    _ReadLog(root, sons);

    file.close();
    return true;
}

void Category::_ReadLog(Node* parentNode, QJsonArray* brothers)
{
    int i, brotherNum = brothers->count();
    QJsonObject obj;
    QJsonArray* sons;
    FCB* fcb;
    Node* newNode,* brotherNode;
    for(i = 0; i < brotherNum; i++)
    {
        // 从JSON中读取此结点的FCB信息
        obj = brothers->at(i).toObject();
        fcb = new FCB(obj.value("fileName").toString(), obj.value("type").toInt());
        fcb->lastModify = obj.value("lastModify").toString();
        fcb->size = obj.value("size").toInt();
        fcb->startBlock = obj.value("startBlock").toInt();

        //建立新结点
        newNode = new Node(fcb);
        // brotherNode
        if(i == 0) // 第一个结点
            parentNode->firstChild = newNode; // 设置父结点的第一个child
        else // 其他结点
            brotherNode->nextBrother = newNode; // 上一个结点的brotherNode为此结点
        if(i == brotherNum - 1) // 最后一个结点
            newNode->nextBrother = nullptr; // brotherNode为空
        brotherNode = newNode;
        // parentNode
        newNode->parent = parentNode;
        // childNode
        if(obj.value("sons").type() == QJsonValue::Null)
            newNode->firstChild = nullptr;
        else
        {
            sons = new QJsonArray;
            *sons = obj.value("sons").toArray();
            _ReadLog(newNode, sons);
        }
    }
    delete brothers;
}

bool Category::WriteLog()
{
    QJsonArray arr;
    QJsonObject obj;
    if(root->firstChild == nullptr)
        obj.insert("Category", QJsonValue::Null);
    else
    {
        _WriteLog(root->firstChild, &arr);
        obj.insert("Category", arr);
    }
    QJsonDocument doc;
    doc.setObject(obj);

    QFile file("log/CategoryLog.json");
    if(!file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    QTextStream fstream(&file);
    fstream << QString(doc.toJson());
    file.close();
    return true;
}

void Category::_WriteLog(Node* node, QJsonArray* parentArr)
{
    QJsonObject* obj = new QJsonObject;
    objStack.push(obj);
    obj->insert("fileName", node->fcb->fileName);
    obj->insert("lastModify", node->fcb->lastModify);
    obj->insert("startBlock", node->fcb->startBlock);
    obj->insert("size", node->fcb->size);
    obj->insert("type", node->fcb->type);

    if(node->firstChild != nullptr)
    {
        QJsonArray* arr = new QJsonArray;
        arrStack.push(arr);
        _WriteLog(node->firstChild, arr);
        obj->insert("sons", *arr);
    }
    else
        obj->insert("sons", QJsonValue::Null);
    parentArr->append(*obj);
    if(node->nextBrother != nullptr)
        _WriteLog(node->nextBrother, parentArr);
}
