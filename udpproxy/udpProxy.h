#ifndef TCPPROXY_H
#define TCPPROXY_H
#include <QtCore>
#include <QTcpServer >
#include <QtNetwork>

class UdpProxy : public QObject 
{
	Q_OBJECT

public:
	UdpProxy(qint32 lport,QHostAddress saddress,qint32 sport,QObject *parent=NULL);
	~UdpProxy();
	public slots:
		void clientReceiveSlot();
		void clientErrorSlot(QAbstractSocket::SocketError);
		void serverReceiveSlot();
		void serverErrorSlot(QAbstractSocket::SocketError);
public slots:
		void dropPacket(int id);
		void sendPacket(int id);
		void changeData(int id);
signals:
	void clientPacketArrive(QString data,int id);
	void serverPacketArrive(QString data,int id);
	void dataChanged(QString data,int id);
protected:
	void removeSocketIfExists(QUdpSocket * socket,QList<QUdpSocket*> & sockets);

	QUdpSocket * getSocket(int id,QList<QUdpSocket*> sockets);
	void processTheDatagram(QHostAddress sender,quint16 senderPort,QByteArray data);
protected:
	QList<QUdpSocket*> m_serverSockets;
	QUdpSocket * m_localServer;
	int m_curId;
	QHostAddress m_saddress;
	qint32 m_sport;
	struct PacktetInfo{
		QUdpSocket * socket;
		QString data;
		QByteArray rawdata;
		int id;
		QHostAddress sendto;
		quint16 port;
	};
	typedef QMap<int,PacktetInfo> PacketMap;
	PacketMap m_packets;
};

#endif // TCPPROXY_H
