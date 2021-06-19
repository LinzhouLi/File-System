#ifndef VIRTUALDISK_H
#define VIRTUALDISK_H

#include <QString>
#include <QVector>
#include "FCB.h"

const int END = -2;

class VirtualDisk
{
private:
    int size;
    int remainBlock;
    int blockSize;
    int blockNum;
    QVector<QString> memory;
    QVector<int> bitMap;
    int GetFileBlockNum(const FCB* fcb);

public:
    VirtualDisk(int size, int blockSize);
    ~VirtualDisk();
    bool AllocMem(FCB* fcb, const QString& content);
    QString GetFileContent(const FCB* fcb);
    void DeleteFileContent(const FCB* fcb);
    bool UpdateFileContent(FCB* fcb, const QString& content);
    bool ReadLog();
    bool WriteLog();
    void Format();
};

#endif // VIRTUALDISK_H
