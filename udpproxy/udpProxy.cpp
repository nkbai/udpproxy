#include "udpProxy.h"
#include <QtNetwork>
#include "config.h"

static quint16 s_startPort=52862;

UdpProxy::UdpProxy(qint32 lport,QHostAddress saddress,qint32 sport,QObject *parent)
: QObject (parent),m_saddress(saddress),m_sport(sport)
{
	m_curId=1;
	m_localServer=new QUdpSocket(this);
	m_localServer->bind(lport);
	connect(m_localServer, SIGNAL(readyRead()),
		this, SLOT(clientReceiveSlot()));
	connect(m_localServer,SIGNAL(error ( QAbstractSocket::SocketError)),this,SLOT(clientErrorSlot(QAbstractSocket::SocketError)));
}

UdpProxy::~UdpProxy()
{
	for(int i=0;i<m_serverSockets.size();i++)
		m_serverSockets[i]->deleteLater();
}
void UdpProxy::processTheDatagram(QHostAddress sender,quint16 senderPort,QByteArray data)
{
	
	qDebug()<<"client from "<<sender<<":"<<senderPort<<":"<<data;
	QUdpSocket * ss=NULL;
	foreach(QUdpSocket * u,m_serverSockets){
		if(u->property("address").toString()==sender.toString()
			&&u->property("port").toUInt()==senderPort){
				ss=u;
				break;
		}
	}
	if(!ss){
		ss=new QUdpSocket(this);
		ss->bind(s_startPort++);
		ss->setProperty("id",m_curId);
		ss->setProperty("address",sender.toString());
		ss->setProperty("port",senderPort);
		//QHostAddress saddress("182.50.1.21");
		//ss->connectToHost(m_saddress,m_sport);
		m_serverSockets.append(ss);
		connect(ss,SIGNAL(readyRead ()),this,SLOT(serverReceiveSlot()));
		connect(ss,SIGNAL(error ( QAbstractSocket::SocketError)),this,SLOT(serverErrorSlot(QAbstractSocket::SocketError)));
	}
	
	PacktetInfo info;
	info.data=data.mid(HEADERLENGTH); //
	info.rawdata=data;
	info.id=m_curId;
	info.port=m_sport;
	info.sendto=m_saddress;
	info.socket=ss;
	m_packets.insert(m_curId,info);
	emit clientPacketArrive(info.data,m_curId);
	m_curId++;
	//ss->writeDatagram(data,m_saddress,m_sport);
}
void UdpProxy::serverReceiveSlot()
{
	QUdpSocket * ss=qobject_cast<QUdpSocket*>(sender());
	
	QHostAddress address=QHostAddress(ss->property("address").toString());
	quint32 port=ss->property("port").toUInt();
	while(ss->hasPendingDatagrams()){
		QByteArray datagram;
		datagram.resize(ss->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;

		ss->readDatagram(datagram.data(), datagram.size(),
			&sender, &senderPort);
		qDebug()<<"server "<<ss->peerAddress()<<":"<<ss->peerPort()<<":"<<datagram;
		if(sender==m_saddress && senderPort==m_sport)
		{
			PacktetInfo info;
			info.data=QString::number(datagram.at(0));
			info.rawdata=datagram;
			info.id=m_curId;
			info.port=port;
			info.sendto=address;
			info.socket=ss;
			m_packets.insert(m_curId,info);
			emit serverPacketArrive(info.data,m_curId);
			m_curId++;
			//ss->writeDatagram(datagram,address,port);
		}
	}

}

void UdpProxy::clientReceiveSlot()
{
	while (m_localServer->hasPendingDatagrams()) {
		QByteArray datagram;
		datagram.resize(m_localServer->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;

		m_localServer->readDatagram(datagram.data(), datagram.size(),
			&sender, &senderPort);

		processTheDatagram(sender,senderPort,datagram);
	}


}





void UdpProxy::clientErrorSlot(QAbstractSocket::SocketError )
{
	QUdpSocket * cs=qobject_cast<QUdpSocket*>(sender());
	qDebug()<<"client error "<<cs->peerAddress()<<":"<<cs->peerPort();
	qDebug()<<cs->errorString();
}







void UdpProxy::serverErrorSlot(QAbstractSocket::SocketError )
{
	QUdpSocket * ss=qobject_cast<QUdpSocket*>(sender());
	qDebug()<<"server error "<<ss->peerAddress()<<":"<<ss->peerPort();
	qDebug()<<ss->errorString();
}

QUdpSocket * UdpProxy::getSocket( int id,QList<QUdpSocket*> sockets )
{
	for(int i=0;i<sockets.size();i++)
	{
		int cid=sockets[i]->property("id").toInt();
		if(cid==id)
			return sockets[i];
	}
	return NULL;
}


void UdpProxy::removeSocketIfExists( QUdpSocket * socket,QList<QUdpSocket*> & sockets )
{
	int id=socket->property("id").toInt();
	for(int i=0;i<sockets.size();i++)
	{
		int cid=sockets[i]->property("id").toInt();
		if(cid==id)
		{
			qDebug()<<"remove :"<<id;
			sockets[i]->close();
			if(&sockets==&m_serverSockets)
				sockets[i]->deleteLater();
			sockets.removeAt(i);
		}
	}
	return;
}

void UdpProxy::dropPacket( int id )
{
	m_packets.remove(id);
}

void UdpProxy::sendPacket( int id )
{
	if(!m_packets.contains(id)){
		qDebug()<<"doesn't exits packet:"<<id;
		return;
	}
	PacktetInfo info=m_packets[id];
	info.socket->writeDatagram(info.rawdata,info.sendto,info.port);
	m_packets.remove(id);
}

void UdpProxy::changeData( int id )
{
	if(!m_packets.contains(id)){
		qDebug()<<"doesn't exits packet:"<<id;
		return;
	}
	QString data=m_packets[id].data;
	ushort a=data.at(0).unicode()+1;
	data[0]=QChar(a);
	m_packets[id].data=data;
	int index=HEADERLENGTH;
	if(m_packets[id].rawdata.size()<=HEADERLENGTH){ //ÕâÖ»ÊÇack
		index=0;
	}
	m_packets[id].rawdata[index]=m_packets[id].rawdata.at(index)+1;
	qDebug()<<"datachanged:"<<data<<":"<<id;
	emit dataChanged(data,id);
}

