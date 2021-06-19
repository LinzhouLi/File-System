#ifndef FCB_H
#define FCB_H

#include <QString>

const int TXTFILE = 0; // 文本文件
const int FOLDER = 1; // 文件夹

const int EMPTY = -1;

class FCB
{
public:
    QString fileName; // 文件名
    QString lastModify; // 上次修改时间
    int startBlock; // 起始块号
    int type; // 文件类型（文件夹或TXT）
    int size; // 文件大小

    FCB(QString name);
    FCB(QString name, int type);
    void SetTime(); // 设置修改时间
};

#endif // FCB_H
