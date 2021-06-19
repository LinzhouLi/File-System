#ifndef FCB_H
#define FCB_H

#include <QString>

const int TXTFILE = 0; // 文本文件
const int FOLDER = 1; // 文件夹

const int EMPTY = -1;

class FCB
{
public:
    QString fileName;
    QString lastModify;
    int startBlock;
    int type;
    int size;

    FCB(QString name);
    FCB(QString name, int type);
    void SetTime();
};

#endif // FCB_H
