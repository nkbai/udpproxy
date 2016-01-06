#include <QtGui/QApplication>
#include "UDP_QT.h"

int main(int argc, char *argv[])
{  
 UDP_QT Server;
 QCoreApplication a(argc, argv);
 Server.InitSocket();
 Server.Send();
    return a.exec();
}