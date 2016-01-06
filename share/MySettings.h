#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QSettings>

class MySettings : public QSettings
{
	Q_OBJECT

public:
	MySettings(const QString & fileName,QObject *parent=NULL);
	~MySettings();
	bool getBoolValue(const QString & key,bool defaultValue=true);
	int getIntValue(const QString & key,int defaultValue=0);
	QString getStringValue(const QString & key,QString defaultValue="");
private:
	
};
MySettings * getSetting();
#endif // MYSETTINGS_H
