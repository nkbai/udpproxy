#include "MySettings.h"

MySettings::MySettings(const QString & fileName,QObject *parent)
: QSettings(fileName,QSettings::IniFormat,parent)
{
	
}

MySettings::~MySettings()
{

}

QString MySettings::getStringValue( const QString & key,QString defaultValue/*=""*/ )
{
	return value(key,defaultValue).toString();
}

int MySettings::getIntValue( const QString & key,int defaultValue/*=0*/ )
{
	return value(key,defaultValue).toInt();
}

bool MySettings::getBoolValue( const QString & key,bool defaultValue/*=true*/ )
{
	QVariant v=value(key);
	if(!v.isValid())
		return defaultValue;
	QString s=v.toString().toLower();
	return s=="true";
}
MySettings * getSetting()
{
	static MySettings * s_settings=NULL;
	if(!s_settings)
	{
		s_settings=new MySettings("conf.ini",NULL);
	}
	return s_settings;
}