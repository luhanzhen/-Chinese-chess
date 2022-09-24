#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qpainter.h>
#include <qlabel.h>
#include <QMouseEvent>
#include <qrect.h>
#include <qbitmap.h>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>
#include <math.h>
#include <QTimer>
#include <QInputDialog>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QByteArray>
#include <QString>
#include <QDataStream>
#include <QIODevice>
#include <QThread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
    bk = QPixmap("bk.png");
    ele = PixmapToRound(QPixmap("ele.png"),20);
    focus_ele = PixmapToRound(QPixmap("ele.png"),22);
    Pix = bk;


    //-----------------------------------------------------------------------------
    bool isOK = true;
    while(isOK)
    {

        QString text = QInputDialog::getText(NULL, "连接",
                                            "输入对方的IP\n取消即等待被连接",
                                            QLineEdit::Normal,          //输入框明文
                                            NULL,
                                            &isOK);

        if(isOK) {

             QRegExp  rx("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
               if( !rx.exactMatch(text) ) //判断是否是有效的IP地址
               {
                     QMessageBox::information(this, tr("错误"), tr("ip地址错误"));
                     isOK = true;
               }
               else
               {

                   isOK = false;
                   sender.setAddress(text);
                   rec_sen_flag = 1;
                   step = 1;

                   SendConnted();

               }

            //  QMessageBox::information(NULL, "Information",   "Your comment is: <b>" + text + "</b>",  QMessageBox::Yes | QMessageBox::No,   QMessageBox::Yes);
        }
        else
        {
             rec_sen_flag = 2;
             step = 2;

             RecConneted();


        }
    }

    InitPix();
    DrawElement();
    //-------------------------------------------------
    //bk = QPixmap("C:\\Users\\luhan\\Desktop\\Chess\\bk.png");
   // ele = PixmapToRound(QPixmap("C:\\Users\\luhan\\Desktop\\Chess\\ele.png"),20);
   // focus_ele = PixmapToRound(QPixmap("C:\\Users\\luhan\\Desktop\\Chess\\ele.png"),22);

}

void MainWindow::RecConneted()
{
     ui->label->setText("等待连接....");
    SerRec=new QTcpServer(this);
    quint16 port = 12501;
    SerRec->listen(QHostAddress::Any,port);
    connect(SerRec,SIGNAL(newConnection()),this,SLOT(newConnection()));

}

void MainWindow::newConnection()
{
    TcpRec=SerRec->nextPendingConnection();

    QMessageBox::about(this,"提示","有人请求对弈一局！");

    //QMessageBox::question(this, "提示", "有人请求对弈一局！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);



    QTimer *testTimer = new QTimer(this);
    QFont ft;
    ft.setPointSize(12);
    ui->label->setFont(ft);
    connect( testTimer,SIGNAL(timeout()), this, SLOT(SeTTime()) );
    testTimer->start(1000); //开始运行定时器，定时时间间隔为1000ms
    connect(TcpRec,SIGNAL(readyRead()),this,SLOT(ReceiveData()));

}

void MainWindow::ReceiveData()
{
    QByteArray arr=TcpRec->readAll();
    QDataStream dst(arr);
    QString flag,stx,sty,enx,eny;

    dst>>flag>>stx>>sty>>enx>>eny;

    if(flag == "surrender")
    {
        QMessageBox::about(this,"提示","对方认输，你赢了！！！");
        //是否开始下一局

    }
    else if(flag == "move")
    {

        Opponent(stx,sty,enx,eny);
    }
    else  if(flag == "checkmate")
    {
        Opponent(stx,sty,enx,eny);
        QMessageBox::about(this,"提示", "将军！");
    }
}

void MainWindow::Opponent(QString stx,QString sty,QString enx,QString eny)
{
    // QMessageBox::about(this,"提示",st + " + "  + en);
    QPoint s,e;
    s.setX(9 - stx.toInt());
    s.setY(8 - sty.toInt());
    e.setX(9 - enx.toInt());
    e.setY(8 - eny.toInt());
    if(board[e.x()][e.y()] > 100)
    {
        if(board[e.x()][e.y()] == 105)
        {
            QMessageBox::about(this,"提示", "你输了！");

        }
        QPainter pp(&Eated_pix);
        pp.drawPixmap((eated_x % 4)* 45,(eated_x / 4)* 45,ele);
        QFont font;
        font.setFamily("Microsoft YaHei");
        font.setPointSize(20);
        pp.setPen(QColor(Qt::red));//设置画笔颜色为红色
        pp.setFont(font);
        pp.drawText(QRectF((eated_x % 4)* 45 - 20 + begin_x,(eated_x / 4)* 45 - 20 + begin_y,40,40),Qt::AlignHCenter,e_blue.mid(board[e.x()][e.y()]-101,1));
        eated_x++;

    }

    board[e.x()][e.y()] =  board[s.x()][s.y()];
    board[s.x()][s.y()] = 0;

    Pix = bk;
    step++;
    DrawElement();


    QPainter eatedp(&Eated_pix);

    eatedp.drawPixmap(((eated_x) % 4)* 45 ,((eated_x ) / 4)* 45,Black_pix);


    QPainter eatp(&Eat_pix);
    QFont fo;
    fo.setFamily("Microsoft YaHei");
    fo.setPointSize(20);
    eatp.setPen(QColor(Qt::red));//设置画笔颜色为红色
    eatp.setFont(fo);
    eatp.drawText(QRectF(((eat_x) % 4)* 45 - 20 + begin_x,((eat_x) / 4)* 45 - 20 + begin_y,40,40),Qt::AlignHCenter,"?");

    repaint();



}


//---------------------------------------------
void MainWindow::connected()
{
    QMessageBox::about(this,"提示","对方同意对弈请求！");
    //QMessageBox::question(this, "提示", "对方同意对弈请求！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    QTimer *testTimer = new QTimer(this);
    QFont ft;
    ft.setPointSize(12);
    ui->label->setFont(ft);
    connect( testTimer,SIGNAL(timeout()), this, SLOT(SeTTime()) );
    testTimer->start(1000); //开始运行定时器，定时时间间隔为1000ms

    connect(TcpSen,SIGNAL(readyRead()),this,SLOT(readyread()));

}

void MainWindow::readyread()
{
    //QMessageBox::about(this,"提示","准备读取");
    QByteArray arr=TcpSen->readAll();
    QDataStream * dst=new QDataStream(&arr,QIODevice::ReadOnly);/******重点******/
    QString flag,stx,sty,enx,eny;
    (*dst)>>flag>>stx>>sty>>enx>>eny;
    if(flag == "surrender")
    {
        QMessageBox::about(this,"提示","对方认输，你赢了！！！");
         //是否开始下一局

    }
    else if(flag == "move")
    {

        Opponent(stx,sty,enx,eny);
    }
}

void MainWindow::SendConnted()
{
     ui->label->setText("等待对方回应....");
     TcpSen=new QTcpSocket(this);
     quint16 port = 12501;
     TcpSen->connectToHost(sender,port,QTcpSocket::ReadWrite);
     connect(TcpSen,SIGNAL(connected()),this,SLOT(connected()));
}


void MainWindow::SeTTime()
{
    //QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
   // QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");//设置系统时间显示格式
    time++;
    QString s = "时间 ";
    s += QString::number(time / 60);
    s += " : ";
    s += QString::number(time % 60);

    ui->label->setText(s);


}

MainWindow::~MainWindow()
{
    delete ui;
}

QPixmap MainWindow::PixmapToRound(const QPixmap &src, int radius) //方形图片转圆形
{
    if (src.isNull())
        return QPixmap();
    QSize size(2*radius, 2*radius);
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(0, 0, size.width(), size.height(), 99, 99);
    QPixmap image = src.scaled(size);
    image.setMask(mask);
    return image;
}


void MainWindow::InitPix() //初始化棋盘
{
    Pix = QPixmap(444,496);
    Pix.fill(Qt::white);

    Eat_pix = QPixmap(180,180);
    Eat_pix.fill(Qt::transparent);

    Eated_pix = QPixmap(180,180);
    Eated_pix.fill(Qt::transparent);

    Black_pix = QPixmap(40,40);
    QPalette pal = this->palette();
    Black_pix.fill(pal.background().color());

    QPainter pp(&Pix);

    pp.setRenderHint(QPainter::Antialiasing, true);

       // 绘制图标
    pp.drawPixmap(0,0, bk);

    for(int i = 0;i < 10;i++)
   {
        for(int j = 0;j<9;j++)
          {

            board[i][j] = 0;

        }

    }
    board[0][0] = 1;board[0][1] = 2;board[0][2] = 3;board[0][3] = 4;board[0][4] = 5;
    board[0][8] = 1;board[0][7] = 2;board[0][6] = 3;board[0][5] = 4;
    board[2][1] = 6;board[2][7] = 6;
    board[3][0] = 7;board[3][2] = 7; board[3][4] = 7;board[3][6] = 7; board[3][8] = 7;
     //------------------------------------------------------------------------------
    board[9][0] = 101;board[9][1] = 102;board[9][2] = 103;board[9][3] = 104;board[9][4] = 105;
    board[9][8] = 101;board[9][7] = 102;board[9][6] = 103;board[9][5] = 104;
    board[7][1] = 106;board[7][7] = 106;
    board[6][0] = 107;board[6][2] = 107; board[6][4] = 107;board[6][6] = 107; board[6][8] = 107;

}

void MainWindow::DrawElement() //绘制元素
{
    QPainter pp(&Pix);
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(20);

    for(int i = 0;i < 10;i++)
    {
        for(int j = 0;j<9;j++)
        {
            if(board[i][j] > 0 && board[i][j] < 100)
            {
                pp.setPen(QColor(Qt::red));//设置画笔颜色为红色
                pp.setFont(font);
                pp.drawPixmap(begin_x + j * diff-20,begin_y + i * diff-20,ele);
                pp.drawText(QRectF(begin_x + j * diff-20,begin_y + i * diff-20,40,40),Qt::AlignHCenter,e_red.mid(board[i][j]-1,1));
               // pp.setPen(Qt::black);
           }
           else if(board[i][j] > 100 && board[i][j] < 1000)
           {
                pp.setPen(QColor(Qt::gray));//设置画笔颜色为红色
                pp.setFont(font); 
                 pp.drawPixmap(begin_x + j * diff-20,begin_y + i * diff-20,ele);
                 pp.drawText(QRectF(begin_x + j * diff-20,begin_y + i * diff-20,40,40),Qt::AlignHCenter,e_blue.mid(board[i][j]-101,1));
                // pp.setPen(Qt::black);
            }

         }
    }
    QFont fo;
    fo.setFamily("Microsoft YaHei");
    fo.setPointSize(20);
    if(step == 1)
    {
        QPainter eatedp(&Eated_pix);
       // eatedp.drawPixmap(((eated_x + 1) % 4)* 45 - 20 + begin_x,((eated_x + 1) / 4)* 45 - 20 + begin_y,Black_pix);


        eatedp.setPen(QColor(Qt::gray));//设置画笔颜色为红色
        eatedp.setFont(fo);
        eatedp.drawText(QRectF(((eated_x) % 4)* 45 - 20 + begin_x,((eated_x) / 4)* 45 - 20 + begin_y,40,40),Qt::AlignHCenter,"?");
    }
    else if(step == 2)
    {
        QPainter eatp(&Eat_pix);
       // eatedp.drawPixmap(((eated_x + 1) % 4)* 45 - 20 + begin_x,((eated_x + 1) / 4)* 45 - 20 + begin_y,Black_pix);

        eatp.setPen(QColor(Qt::red));//设置画笔颜色为红色
        eatp.setFont(fo);
        eatp.drawText(QRectF(((eat_x) % 4)* 45 - 20 + begin_x,((eat_x) / 4)* 45 - 20 + begin_y,40,40),Qt::AlignHCenter,"?");

    }

}

void MainWindow::UpdatePix() //更新
{
    QPainter painter(this);
    painter.drawPixmap(0,11,Pix);
    painter.drawPixmap(450,280,Eat_pix);
    painter.drawPixmap(450,30,Eated_pix);

}

void MainWindow::paintEvent(QPaintEvent *event) //绘图
{
    Q_UNUSED(event);
    UpdatePix();

}


QPoint MainWindow::GetLoc(QPoint p) //获取点击鼠标的位置坐标
{
    QPoint ret(-1,-1);
    //bool flag = false;
    for(int i = 0; i < 10;i++)
    {
        for(int j = 0; j < 9;j++)
        {
            if(board[i][j] != 0 && board[i][j] > 100)
            {
                if((begin_x + j * diff - p.x())*(begin_x + j * diff - p.x()) +(begin_y + i * diff-p.y()) *  (begin_y + i * diff-p.y()) < 400)
                {
                   // flag = true;
                    ret.setY(j);
                    ret.setX(i);

                }
            }
        }
    }

    return ret;
}

//---点击左键
void MainWindow::mousePressEvent(QMouseEvent *e)
{

    if (Qt ::LeftButton == e->button())
    {
        if(e->pos().x() > 445)
            return ;
        if(step%2 == 1)
            return ;
        QPoint p = GetLoc(e->pos());
        if(p.x() == -1 && p.y() == -1)
            return ;
        start = p;


        focus = 1;
        //repaint();
       // outputBoard();
        //QMessageBox::information(NULL, "Title", s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    }
}


QPoint MainWindow::GetLocation(QPoint p)  //获取落子的位置
{
    QPoint ret(-1,-1);
     if(p.x() > 445)
         ret.setY(8);
     else if(p.x() < 0)
         ret.setY(0);
     else
        ret.setY((p.x()-0)/50);
    if(p.y() < 65)
        ret.setX(0);
    else if( p.y() > 496)
        ret.setX(9);
    else
        ret.setX((p.y()-11)/50);




    return ret;
}
//---释放左键
void MainWindow::outputBoard() //调试输出棋盘
{

    for(int i = 0; i < 10;i++)
    {

            qDebug("%d %d %d %d %d %d %d %d %d",board[i][0],board[i][1],board[i][2],board[i][3],board[i][4],board[i][5],board[i][6],board[i][7],board[i][8]);


    }
    qDebug("---------------------------------------");

}

bool MainWindow::allow(QPoint s,QPoint p,int f)  //允许落子
{
    bool ret = false;
    int t = -1;
    switch(board[s.x()][s.y()]-f)
    {
    case 1:
        t = Rooks(s,p);
        break;
    case 2:

        t = Knight(s,p);


        break;
    case 3:

        t = Elephants(s,p);

        break;
    case 4:
        t = Mandarins(s,p);

        break;
    case 5:
        t = King(s,p);

        break;
    case 6:
        t = Cannons(s,p);

        break;
    case 7:
        t = Pawns(s,p);

        break;
    }
    if( t == 1)     //正常落子
        ret = true;
    else if(t == 2) //落子且吃子
    {
        ret = true;
        Eat(p);

    }
    else                 //落子不规范
        ret = false;
    return ret;
}

void MainWindow::Eat(QPoint p)
{

    QPainter pp(&Eat_pix);
    pp.drawPixmap((eat_x % 4)* 45,(eat_x / 4)* 45,ele);
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(20);
    pp.setPen(QColor(Qt::red));//设置画笔颜色为红色
    pp.setFont(font);
    pp.drawText(QRectF((eat_x % 4)* 45 - 20 + begin_x,(eat_x / 4)* 45 - 20 + begin_y,40,40),Qt::AlignHCenter,e_red.mid(board[p.x()][p.y()]-1,1));
    eat_x++;




}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (Qt ::LeftButton == e->button())
    {
        if(focus != 2)
            return ;
        Pix = temp;
        repaint();

        QPoint p =  GetLocation(e->pos());
        if(p.x() == -1 && p.y() == -1)
            return ;

        if(allow(start,p,1100))
        {
            board[p.x()][p.y()] =  board[start.x()][start.y()] - 1000;
            if(!(p.x() == start.x() && p.y() == start.y())) //动了
                 board[start.x()][start.y()] = 0;

            end = p;
            send();
            if(board[p.x()][p.y()] == 5)
            {
                QMessageBox::about(this,"提示", "你赢了！");

            }


        }
        else
        {
            board[start.x()][start.y()] -= 1000;
            end = start;
        }
        Pix = temp;
        QPainter pp(&Pix);
        pp.drawPixmap(begin_x + end.y() * diff-20,begin_y + end.x() * diff-20,ele);
        QFont font;
        font.setFamily("Microsoft YaHei");
        font.setPointSize(20);
        pp.setPen(QColor(Qt::gray));//设置画笔颜色为红色
        pp.setFont(font);
        pp.drawText(QRectF(begin_x + end.y() * diff-20,begin_y + end.x() * diff-20,40,40),Qt::AlignHCenter,e_blue.mid(board[end.x()][end.y()]-101,1));
        repaint();
        focus = 0;

       // outputBoard();
    }
}

int MainWindow::Checkmate()  //将军！！！
{
    int i,j;
    int kingx = -1,kingy = -1;
    for(i = 0;i < 3 ;i++)
     {
        for(j = 3;j < 6;j++)
         {
            if(board[i][j] == 5)
            {
                kingx = i;
                kingy = j;
            }
          }
      }

    for(i = 0;i < 10;i++)
    {
        for(j = 0;j < 9;j++)
        {
            if(board[i][j] == 101 || board[i][j] == 102 || board[i][j] == 105 || board[i][j] == 106 || board[i][j] == 107)
            {


                if(allow(QPoint(i,j),QPoint(kingx,kingy),100))
                 {

                    return 1;
                }

            }
        }
    }
    return 0;


}


void MainWindow::send()
{

    QByteArray arr;
    QDataStream dst(&arr,QIODevice::ReadWrite);/*QDataStream是继承于QIODevice的所以 必须要 设置读写权限*/
    if(Checkmate() == 1)
        dst<<QString("checkmate")<<QString::number(start.x())<<QString::number(start.y())<<QString::number(end.x())<<QString::number(end.y());/*这里一定要设置好数据类型*/
    else
        dst<<QString("move")<<QString::number(start.x())<<QString::number(start.y())<<QString::number(end.x())<<QString::number(end.y());/*这里一定要设置好数据类型*/
    if(rec_sen_flag == 2)
        TcpRec->write(arr);
    else if(rec_sen_flag == 1)
         TcpSen->write(arr);
    step++;

    //---------------------------------------------

    QPainter eatp(&Eat_pix);

    eatp.drawPixmap(((eat_x) % 4)* 45,((eat_x ) / 4)* 45,Black_pix);

    QPainter eatedp(&Eated_pix);
   // eatedp.drawPixmap(((eated_x + 1) % 4)* 45 - 20 + begin_x,((eated_x + 1) / 4)* 45 - 20 + begin_y,Black_pix);

    QFont fo;
    fo.setFamily("Microsoft YaHei");
    fo.setPointSize(20);
    eatedp.setPen(QColor(Qt::gray));//设置画笔颜色为红色
    eatedp.setFont(fo);
    eatedp.drawText(QRectF(((eated_x) % 4)* 45 - 20 + begin_x,((eated_x) / 4)* 45 - 20 + begin_y,40,40),Qt::AlignHCenter,"?");


}

//--鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(focus && e->buttons() & Qt::LeftButton){  //进行的按位与

        if(focus == 1)
        {

            board[start.x()][start.y()] += 1000;
            Pix = bk;
            DrawElement();
            temp = Pix;
            focus++;
        }
         Pix = temp;
         QPainter pp(&Pix);
         pp.drawPixmap(e->pos().x()-begin_x,e->pos().y()-begin_y-11,focus_ele);
         QFont font;
         font.setFamily("Microsoft YaHei");
         font.setPointSize(21);
         pp.setPen(QColor(Qt::gray));//设置画笔颜色为红色
         pp.setFont(font);
         pp.drawText(QRectF(e->pos().x()-22,e->pos().y()-11-22,44,44),Qt::AlignHCenter,e_blue.mid(board[start.x()][start.y()]-1101,1));
         repaint();
    }

}


void MainWindow::on_pushButton_clicked()
{

    QMessageBox::information(NULL, "关于", "联系我：94864649@qq.com", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

void MainWindow::on_pushButton_2_clicked()
{
    QByteArray arr;
    QDataStream dst(&arr,QIODevice::ReadWrite);/*QDataStream是继承于QIODevice的所以 必须要 设置读写权限*/
    dst<<QString("surrender");/*这里一定要设置好数据类型*/
    if(rec_sen_flag == 2)
        TcpRec->write(arr);
    else if(rec_sen_flag == 1)
         TcpSen->write(arr);

}

 int MainWindow::Rooks(QPoint s,QPoint p) // 车
{
     if(p.x() == s.x() && p.y() != s.y())
     {
         if(p.y() < s.y())
         {
             int i = s.y()-1;
             for(;i > p.y();i--)
             {
                 if(board[p.x()][i] != 0)
                      return 0;
                 else
                      continue;

             }
             if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
                 return 2;
             else if(board[p.x()][p.y()] > 100)
                 return 0;
             else if(board[p.x()][p.y()] == 0)
                 return 1;

         }
         else if(p.y() > s.y())
         {
             int i = s.y()+1;
             for(;i < p.y();i++)
             {
                 if(board[p.x()][i] != 0)
                     return 0;
                 else
                     continue;
             }
             if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
                 return 2;
             else if(board[p.x()][p.y()] > 100)
                 return 0;
             else if(board[p.x()][p.y()] == 0)
                 return 1;
         }

     }
     else if(p.x() != s.x() && p.y() == s.y())
     {
         if(p.x() < s.x())
         {
             int i = s.x()-1;
             for(;i > p.x();i--)
             {
                 if(board[i][p.y()] != 0)
                     return 0;
                 else
                     continue;
             }
             if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
                 return 2;
             else if(board[p.x()][p.y()] > 100)
                 return 0;
             else if(board[p.x()][p.y()] == 0)
                 return 1;

         }
         else if(p.x() > s.x())
         {
             int i = s.x()+1;
             for(;i < p.x();i++)
             {
                 if(board[i][p.y()] != 0)
                     return 0;
                 else
                     continue;
             }
             if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
                 return 2;
             else if(board[p.x()][p.y()] > 100)
                 return 0;
             else if(board[p.x()][p.y()] == 0)
                 return 1;
         }
     }
     else if(p.x() != s.x() && p.y() != s.y())
         return 0;

     return 0;

}

 int MainWindow::Knight(QPoint s,QPoint p) // 马
{
     int d = abs(p.x() - s.x()) + abs(p.y() - s.y());
     if(d != 3)
         return 0;
     else
     {
         if(p.x() == s.x() || p.y() == s.y())
             return 0;
         else
         {
             if(abs(p.x() - s.x()) == 2)
             {
                 if((board[(p.x() + s.x())/2][s.y()]) != 0)
                     return 0;
             }
             else if(abs(p.y() - s.y()) == 2)
             {
                 if((board[s.x()][(p.y() + s.y())/2]) != 0)
                     return 0;
             }
             if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
                 return 2;
             else if(board[p.x()][p.y()] > 100)
                 return 0;
             else if(board[p.x()][p.y()] == 0)
                 return 1;
         }

     }
     return 0;
}

 int MainWindow::Elephants(QPoint s,QPoint p) // 象
{


     if(p.x() < 5)
         return 0;
     int d = abs(p.x() - s.x()) + abs(p.y() - s.y());

     if(d != 4)
         return 0;
     else
     {

         if(p.x() == s.x() || p.y() == s.y())
             return 0;
         if((board[(p.x() + s.x())/2][(p.y() + s.y())/2]) != 0)
             return 0;

         if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
             return 2;
         else if(board[p.x()][p.y()] > 100)
             return 0;
         else if(board[p.x()][p.y()] == 0)
             return 1;

     }
     return 0;
}

 int MainWindow::Mandarins(QPoint s,QPoint p) // 士
{
     int d = abs(p.x() - s.x()) + abs(p.y() - s.y());

     if(d != 2)
         return 0;
     else
     {
         if(p.x() < 7 || p.y() < 3 || p.y() > 5)
             return 0;
         if(p.x() == s.x() || p.y() == s.y())
             return 0;
         if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
             return 2;
         else if(board[p.x()][p.y()] > 100)
             return 0;
         else if(board[p.x()][p.y()] == 0)
             return 1;
     }
     return 0;

}

 int MainWindow::King(QPoint s,QPoint p) // 将
{
     if(board[p.x()][p.y()] == 5 && p.y() == s.y())  //两将碰面
     {

         int n = 0;
         for(int i = p.x() + 1;i < s.x();i++)
             if(board[i][p.y()] != 0)
                 n++;
         //QMessageBox::about(this,"提示", "  ！");
         if(n == 0)
         {
             return 2;
         }
         else
             return 0;
     }

     int d = abs(p.x() - s.x()) + abs(p.y() - s.y());
     if(d != 1)
         return 0;
     else
     {
         if(p.x() < 7 || p.y() < 3 || p.y() > 5)
             return 0;


         if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
             return 2;
         else if(board[p.x()][p.y()] > 100)
             return 0;
         else if(board[p.x()][p.y()] == 0)
             return 1;
     }
     return 0;
}

int MainWindow::Cannons(QPoint s,QPoint p) // 炮
{
    int n = 0;
    if(p.x() == s.x() && p.y() != s.y())
    {
        if(p.y() < s.y())
        {
            if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
            {
                int i = s.y()-1;
                for(;i > p.y();i--)
                {
                    if(board[p.x()][i] != 0)
                        n++;
                }
                if(n == 1)  //保证炮只隔着一个
                    return 2;
                else
                    return 0;
            }
            else
            {
                int i = s.y()-1;
                for(;i > p.y();i--)
                {
                    if(board[p.x()][i] != 0)
                         return 0;
                    else
                         continue;
                }
                return 1;
            }

        }
        else if(p.y() > s.y())
        {
            if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
            {
                int i = s.y()+1;
                for(;i < p.y();i++)
                {
                    if(board[p.x()][i] != 0)
                        n++;
                }
                if(n == 1)  //保证炮只隔着一个
                    return 2;
                else
                    return 0;
            }
            else
            {
                int i = s.y()+1;
                for(;i < p.y();i++)
                {
                    if(board[p.x()][i] != 0)
                         return 0;
                    else
                         continue;
                }
                return 1;
            }

        }

    }
    else if(p.x() != s.x() && p.y() == s.y())
    {
        if(p.x() < s.x())
        {
            if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
            {
                int i = s.x()-1;
                for(;i > p.x();i--)
                {
                    if(board[i][p.y()] != 0)
                        n++;
                }
                if(n == 1)  //保证炮只隔着一个
                    return 2;
                else
                    return 0;
            }
            else
            {
                int i = s.x()-1;
                for(;i > p.x();i--)
                {
                    if(board[i][p.y()] != 0)
                         return 0;
                    else
                         continue;
                }
                return 1;
            }

        }
        else if(p.x() > s.x())
        {
            if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
            {
                int i = s.x()+1;
                for(;i < p.x();i++)
                {
                    if(board[i][p.y()] != 0)
                        n++;
                }
                if(n == 1)  //保证炮只隔着一个
                    return 2;
                else
                    return 0;
            }
            else
            {
                int i = s.x()+1;
                for(;i < p.x();i++)
                {
                    if(board[i][p.y()] != 0)
                         return 0;
                    else
                         continue;
                }
                return 1;
            }

        }
    }
    else if(p.x() != s.x() && p.y() != s.y())
        return 0;

    return 0;

}

 int MainWindow::Pawns(QPoint s,QPoint p) // 兵
{
     int d = abs(p.x() - s.x()) + abs(p.y() - s.y());
     if(d != 1)
         return 0;
     else
     {
         if(p.x() > 4 && p.x() == s.x())
             return 0;
         if(p.x() > s.x())
             return 0;
         if(board[p.x()][p.y()] < 100 && board[p.x()][p.y()] > 0)
             return 2;
         else if(board[p.x()][p.y()] > 100)
             return 0;
         else if(board[p.x()][p.y()] == 0)
             return 1;
     }
     return 0;

}


