#include "VirtualDisk.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

// 初始化虚拟硬盘
VirtualDisk::VirtualDisk(int size, int blockSize)
{
    this->size = size;
    this->blockSize = blockSize;
    this->blockNum = size / blockSize;
    this->remainBlock = blockNum;

    for(int i = 0; i < blockNum; i++)
    {
        bitMap.append(EMPTY);
        memory.append("");
    }
}

VirtualDisk::~VirtualDisk()
{

}

// 得到文件占用存储块数量
int VirtualDisk::GetFileBlockNum(const FCB* fcb)
{
    int size = fcb->size;
    size = size / blockSize + (size % blockSize == 0 ? 0 : 1);
    if(size == 0) size = 1;
    return size;
}

// 为文件分配存储块
bool VirtualDisk::AllocMem(FCB* fcb, const QString& content)
{
    fcb->SetTime();
    fcb->size = content.length();
    int blocks = GetFileBlockNum(fcb);
    int i, j;
    int last = 0; // 记录上一个块的地址
    if(blocks > remainBlock)
        return false;
    else
    {
        // 找到文件可以开始存放的位置
        for(i = 0; i < blockNum; i++)
        {
            if(bitMap[i] == EMPTY)
            {
                remainBlock--;
                fcb->startBlock = i;
                memory[i] = content.mid(0, blockSize);
                last = i;
                break;
            }
        }

        // 找到后续存放的位置
        for(j = 1, i++; i < blockNum && j < blocks; i++)
        {
            if(bitMap[i] == EMPTY)
            {
                remainBlock--;

                bitMap[last] = i; // 以链接的方式组织
                last = i;

                memory[i] = content.mid(j * blockSize, blockSize);
                j++;
            }
        }

        // 设置最后一个块
        bitMap[i - 1] = END;
        return true;
    }
}

// 从虚拟硬盘中得到文件内容
QString VirtualDisk::GetFileContent(const FCB* fcb)
{
    int last = fcb->startBlock; // last表示上一个块的地址，初始化为起始块地址
    QString content = "";

    // 如果文件为空
    if(last == EMPTY)
        return content;

    // 文件不为空
    while(last != END)
    {
        content += memory[last];
        last = bitMap[last];
    }
    return content;
}

// 从虚拟硬盘中删除某文件内容
void VirtualDisk::DeleteFileContent(const FCB* fcb)
{
    if(fcb->type == FOLDER) return;
    int last = fcb->startBlock; // last表示上一个块的地址，初始化为起始块地址
    int temp;

    while(last != END && last != EMPTY)
    {
        memory[last] = "";
        temp = bitMap[last];
        bitMap[last] = EMPTY;
        last = temp;
        remainBlock++;
    }
}

// 从虚拟硬盘中更新某文件内容
bool VirtualDisk::UpdateFileContent(FCB* fcb, const QString& content)
{
    DeleteFileContent(fcb);
    fcb->size = content.length();
    return AllocMem(fcb, content);
}

bool VirtualDisk::ReadLog()
{
    // 读Json文件
    QFile file("log/Disk.json");
    if(!file.open(QIODevice::Text | QIODevice::ReadOnly)) // 文件读取错误
        return false;
    QJsonParseError err; // 错误标识
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if(err.error != QJsonParseError::NoError) // JSON格式错误
        return false;

    // 生成disk
    QJsonObject disk = doc.object();
    QJsonArray memoryJSON, bitMapJSON;
    this->size = disk.value("size").toInt();
    this->remainBlock = disk.value("remainBlock").toInt();
    this->blockSize = disk.value("blockSize").toInt();
    this->blockNum = disk.value("blockNum").toInt();
    memoryJSON = disk.value("memory").toArray();
    bitMapJSON = disk.value("bitMap").toArray();
    memory.resize(blockNum);
    bitMap.resize(blockNum);
    for(int i = 0; i < blockNum; i++)
    {
        this->memory[i] = memoryJSON.at(i).toString();
        this->bitMap[i] = bitMapJSON.at(i).toInt();
    }

    file.close();
    return true;
}

bool VirtualDisk::WriteLog()
{
    // 打开JSON文件
    QFile file("log/Disk.json");
    if(!file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    QTextStream fstream(&file);

    // 生成JSON格式数据
    QJsonObject disk;
    QJsonArray memoryJSON, bitMapJSON;
    QJsonDocument doc;
    disk.insert("size", this->size);
    disk.insert("remainBlock", this->remainBlock);
    disk.insert("blockSize", this->blockSize);
    disk.insert("blockNum", this->blockNum);
    for(int i = 0; i < blockNum; i++)
    {
        memoryJSON.append(memory[i]);
        bitMapJSON.append(bitMap[i]);
    }
    disk.insert("memory", memoryJSON);
    disk.insert("bitMap", bitMapJSON);
    doc.setObject(disk);

    // 写入JSON文件
    fstream << QString(doc.toJson());
    file.close();

    return true;
}

void VirtualDisk::Format()
{
    for(int i = 0; i < blockNum; i++)
    {
        memory[i] = "";
        bitMap[i] = EMPTY;
    }
    remainBlock = blockNum;
}
