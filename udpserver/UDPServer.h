#pragma once

#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <QThread>
 
class UDPServer: public QObject
{
  Q_OBJECT 
public:
 void InitSocket();
 ~UDPServer();
 UDPServer(QObject* parent=NULL);
private slots:
    void Recv();
	void rdt_recv(QByteArray packet);
	QByteArray extract(QByteArray packet);
	void deliver_data(QByteArray data);
	bool notcorrupt(QByteArray pkt);
	void udt_send(QByteArray data);
private:
	QUdpSocket  *udpSocket;   //Ì×½Ó×Ö¶ÔÏó
	QHostAddress m_client;
	quint16 m_clientPort;
};
