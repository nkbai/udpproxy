#include <QtCore>
#include "udpserver.h"
int main(int argc,char ** argv)
{
	QCoreApplication app(argc,argv);
	UDPServer server;
	server.InitSocket();
	return app.exec();
}