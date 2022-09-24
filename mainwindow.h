#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *event);

    void InitPix();
    void DrawElement();

    void UpdatePix();
    QPixmap PixmapToRound(const QPixmap &src, int radius);
    QPoint GetLoc(QPoint p);
    QPoint GetLocation(QPoint p);
    void mousePressEvent(QMouseEvent *e);       //--鼠标按下事件
    void mouseMoveEvent(QMouseEvent *e);    //--鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *e); //--鼠标释放（松开）事件
    void outputBoard();
    bool allow(QPoint s,QPoint p,int f);
    void Eat(QPoint p);
    int Checkmate();
    //-----------------------------------
    void RecConneted();
    void SendConnted();
    void Opponent(QString stx, QString sty, QString enx, QString eny);
    void send();
//-----rule----------------------------------
    int Rooks(QPoint s,QPoint p); // 车
    int Knight(QPoint s,QPoint p); // 马
    int Elephants(QPoint s,QPoint p); // 象
    int Mandarins(QPoint s,QPoint p); // 士
    int King(QPoint s,QPoint p); // 将
    int Cannons(QPoint s,QPoint p); // 炮
    int Pawns(QPoint s, QPoint p); // 兵

private slots:
    void on_pushButton_clicked();
    void SeTTime();
    void newConnection();
    void connected();
    void ReceiveData();
    void readyread();

   // void readPendingDatagram();
    //void RecRequest();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    QPixmap Pix,temp,bk;
    QPixmap ele,focus_ele;
    QPixmap Eat_pix,Eated_pix,Black_pix;
    int eat_x = 0 ,eated_x = 0;
    int begin_x = 22,begin_y = 22,diff = 50;
    int focus = 0;
    int board[10][9];
    QString e_blue = "車馬象士將炮兵"; // 1234567
    QString e_red = "車馬象仕帥炮卒"; // 100+1234567

    QPoint start,end;

    int time = 0; //时间

    int rec_sen_flag = 0;
    QTcpSocket *TcpRec,*TcpSen;
    QTcpServer *SerRec;
    QHostAddress sender;
    int step = 0;

};

#endif // MAINWINDOW_H
