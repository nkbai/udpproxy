#include "UDPServer.h"
#include <QtCore>
#include "config.h"
#include "MySettings.h"

void UDPServer::InitSocket()
{
  udpSocket = new QUdpSocket(this);
  int sport=getSetting()->getIntValue("sport",2002);
  udpSocket->bind(QHostAddress::Any,sport);
  connect(udpSocket, SIGNAL(readyRead()),
   this, SLOT(Recv()));
}
void UDPServer::deliver_data(QByteArray data)
{
	qDebug()<<"recevie:"<<data.mid(HEADERLENGTH);
}
QByteArray  UDPServer::extract(QByteArray packet)
{
	return packet;
}
bool UDPServer::notcorrupt(QByteArray pkt)
{
	QDataStream ds(&pkt,QIODevice::ReadOnly);
	quint16 crc;
	QByteArray data;
	ds>>crc;
	data=ds.device()->readAll();
	if(crc==qChecksum(data.data(),data.size()))
		return true;
	return false;
}
void UDPServer::rdt_recv(QByteArray packet)
{
	char ch=0;
	if(notcorrupt(packet)){
		QByteArray data=extract(packet);
		deliver_data(data);
		ch=1;
	}else
	{
		qDebug()<<"receive corrupt :"<<packet.mid(HEADERLENGTH);
	}
	//send ack
	QByteArray data(1,ch);
	udt_send(data);
}
void UDPServer::udt_send(QByteArray data)
{
	udpSocket->writeDatagram(data,m_client,m_clientPort);
}
void UDPServer::Recv()
{
  while (udpSocket->hasPendingDatagrams())
 {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(),&m_client,&m_clientPort);                            
		rdt_recv(datagram);
    }
}

UDPServer::~UDPServer()
{
	delete udpSocket;
}

UDPServer::UDPServer( QObject* parent/*=NULL*/ )
{
	udpSocket=NULL;
}



