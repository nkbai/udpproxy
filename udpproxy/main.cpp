#include <QtGui>
#include "udpProxy.h"
#include <QtWebKit>
#include "webview.h"
int main(int argc,char ** argv)
{
	QApplication app(argc,argv);
	//UdpProxy proxy(5000,QHostAddress("127.0.0.1"),2002);
	AutoWebView view;
	view.load(QString("test.html"));
	view.setFixedWidth(1000);
	view.show();
	return app.exec();
}