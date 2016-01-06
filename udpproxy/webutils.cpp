#include "webutils.h"
#include <QtGui>
#include <QtWebKit>
#include "udpProxy.h"
#include "MySettings.h"
UdpProxy * WebUtils::m_com=NULL;
WebUtils::WebUtils(QWebPage * parent):QObject(parent)
{
	m_page=parent;
	if(!m_com)
	{
		QThread *t=new QThread;
		t->start(QThread::NormalPriority);
		int localPort=getSetting()->getIntValue("localPort",5000);
		QString saddr=getSetting()->getStringValue("saddr","127.0.0.1");
		int sport=getSetting()->getIntValue("sport",2002);
		m_com=new UdpProxy(localPort,QHostAddress(saddr),sport);
		m_com->moveToThread(t);

		connect(this,SIGNAL(dropPacketSignal(int)),
			m_com,SLOT(dropPacket(int)));
		connect(this,SIGNAL(sendPacketSignal(int)),
			m_com,SLOT(sendPacket(int)));
		connect(this,SIGNAL(changeDataSignal(int)),
			m_com,SLOT(changeData(int)));
		connect(m_com,SIGNAL(clientPacketArrive(QString,int)),this,SLOT(clientPacketArrive(QString,int)));
		connect(m_com,SIGNAL(serverPacketArrive(QString,int)),this,SLOT(serverPacketArrive(QString,int)));
		connect(m_com,SIGNAL(dataChanged(QString,int)),this,SLOT(dataChanged(QString,int)));
	}
}

void WebUtils::clientPacketArrive( QString data,int id )
{
	QString s=QString("addleft('%1',%2);").arg(data).arg(QString::number(id));
	m_page->mainFrame()->evaluateJavaScript(s);
}

void WebUtils::serverPacketArrive( QString data,int id )
{
	QString s=QString("addright('%1',%2);").arg(data).arg(QString::number(id));
	m_page->mainFrame()->evaluateJavaScript(s);
}

void WebUtils::dataChanged( QString data,int id )
{
	QString s=QString("datachange('%1',%2);").arg(data).arg(QString::number(id));
	m_page->mainFrame()->evaluateJavaScript(s);
}

void WebUtils::dropPacket( int id )
{	
	 emit dropPacketSignal(id);
}

void WebUtils::sendPacket( int id )
{
	emit sendPacketSignal(id);
}

void WebUtils::changeData( int id )
{
	emit changeDataSignal(id);
}








