#ifndef RECTHREAD_H
#define RECTHREAD_H

#include <QThread>

class RecThread : public QThread
{
public:

    explicit RecThread(QObject *parent = 0);
   // explicit RecThread(string aa,string bb,int s);
    //explicit RecThread();
    void run();
signals:
    UpdateSignal(QString s);

   // myThread = new MyThread(a,b,status);

     //   connect(myThread, SIGNAL(UpdateSignal(QString)),
      //          this, SLOT(UpdateSlot(QString)));
      //   myThread->start();
};

#endif // RECTHREAD_H
