#ifndef UDP_QT_H
#define UDP_QT_H

#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <QThread>


#pragma once
 
class UDP_QT: public QObject
{

private:


public:
void InitSocket();
void run();
void udt_send(QByteArray data);
QByteArray make_pkt(QByteArray data);
void rdt_send(QByteArray data);
bool rdt_recv();
bool isACK(QByteArray pkt);
void Send();
};
#endif