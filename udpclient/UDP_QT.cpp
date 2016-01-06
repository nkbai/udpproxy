#include "UDP_QT.h"
#include <QtCore>
#include <Windows.h>
#include "MySettings.h"
#include "config.h"
QUdpSocket  *udpSocket;   //套接字对象


char * information[]={"123","456","789"};
void UDP_QT::InitSocket()
{
 udpSocket = new QUdpSocket(this);
 udpSocket->bind(3333);
}
QByteArray UDP_QT::make_pkt(QByteArray data)
{
	QByteArray pkt;
	QDataStream ds(&pkt,QIODevice::WriteOnly);
	quint16 crc=qChecksum(data.data(),data.size());
	ds<<crc;
	ds.writeRawData (data.data(),data.size());
	return pkt;
}
void UDP_QT::udt_send(QByteArray data)
{
	QString saddr=getSetting()->getStringValue("saddr","127.0.0.1");
	int sport=getSetting()->getIntValue("sport",5000);
	udpSocket->writeDatagram(data,QHostAddress(saddr),sport);
	qDebug()<<"send:"<<data.mid(HEADERLENGTH);
}
void UDP_QT::rdt_send(QByteArray data)
{
	QByteArray pkt=make_pkt(data);
	udt_send(pkt);
}
bool UDP_QT::rdt_recv()
{
	QEventLoop loop;
	if(!udpSocket->hasPendingDatagrams()){
		connect(udpSocket,SIGNAL(readyRead()),&loop,SLOT(quit()));
		loop.exec();
	}
	bool ret=false;
	while(udpSocket->hasPendingDatagrams()){
		QByteArray datagram;
		datagram.resize(udpSocket->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;

		udpSocket->readDatagram(datagram.data(), datagram.size(),
			&sender, &senderPort);
		ret=isACK(datagram);
	}
	return ret;
}

bool  UDP_QT::isACK(QByteArray pkt){
	if(pkt.size()>=1 && pkt.at(0)==1)
		return true;
	return false;
}
void UDP_QT::Send()
{

 int i=0;
 while(1)
 {
		QByteArray data(information[i]);
		rdt_send(data);
		bool ret=rdt_recv();
		if(!ret)
			continue;
		 i++;
		 if(i>=sizeof(information)/sizeof(char*))
			 i=0;
		Sleep(1000); //sleep函数要在进程下才能使用,它继承QThread
 }
}
void UDP_QT::run() //运行进程
{
  Send();
}

