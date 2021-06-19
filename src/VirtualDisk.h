#ifndef VIRTUALDISK_H
#define VIRTUALDISK_H

#include <QString>
#include <QVector>
#include "FCB.h"

const int END = -2;

class VirtualDisk
{
private:
    int size; // disk总容量
    int remainBlock; // 剩余块数
    int blockSize; // 块大小
    int blockNum; // 块数
    QVector<QString> memory; // disk存储空间
    QVector<int> bitMap; // 位图
    int GetFileBlockNum(const FCB* fcb); // 获得某文件需要的块数

public:
    VirtualDisk(int size, int blockSize); // 初始化磁盘，须提供总容量与块大小
    ~VirtualDisk();
    bool AllocMem(FCB* fcb, const QString& content); // 为某文件分配存储块
    QString GetFileContent(const FCB* fcb); // 读取某文件内容
    void DeleteFileContent(const FCB* fcb); // 删除某文件内容
    bool UpdateFileContent(FCB* fcb, const QString& content); // 更新某文件内容
    bool ReadLog(); // 读日志文件
    bool WriteLog(); // 写日志文件
    void Format(); // 格式化
};

#endif // VIRTUALDISK_H
