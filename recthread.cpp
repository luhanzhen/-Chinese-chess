#include "recthread.h"


RecThread::RecThread(QObject *parent) :
    QThread(parent)
{

}

void RecThread::run()
{
    QString s;
   // emit UpdateSignal(s);
}
