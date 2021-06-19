#include "FCB.h"
#include <QDateTime>

FCB::FCB(QString name)
{
    this->fileName = name;
    this->type = TXTFILE;
    this->startBlock = EMPTY;
    this->size = 0;
    SetTime();
}

FCB::FCB(QString name, int type)
{
    this->fileName = name;
    this->type = type;
    SetTime();
    this->startBlock = EMPTY;
    this->size = 0;
}

void FCB::SetTime()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    lastModify =current_date_time.toString("yyyy.MM.dd hh:mm"); //:ss.zzz ddd
}
