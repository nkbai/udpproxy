#ifndef JUTILS_H
#define JUTILS_H

#include <QObject>
#include <QtCore>
#include <QtWebKit>

class UdpProxy;
class WebUtils : public QObject
{
    Q_OBJECT
public:
    WebUtils(QWebPage * parent);
signals:
	void dropPacketSignal(int id);
	void sendPacketSignal(int id);
	void changeDataSignal(int id);
public slots:
	void clientPacketArrive(QString data,int id);
	void serverPacketArrive(QString data,int id);
	void dataChanged(QString data,int id);

	public slots:
		void dropPacket(int id);
		void sendPacket(int id);
		void changeData(int id);

private:
	
private:
	static UdpProxy * m_com;
	QWebPage * m_page;
};

#endif // JUTILS_H
