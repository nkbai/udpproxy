#include "webview.h"
#include <QWebView>
#include <QtGui>
AutoWebPage::AutoWebPage(QObject *parent): QWebPage(parent)
{
	m_utl=NULL;
      QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,false);
      QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled,true);
#ifdef _DEBUG
	  QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
#endif  
       //QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
      //必须是默认的，否则会出错
	  QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled,true);
	  //QWebSettings::enablePersistentStorage ( QDir::currentPath()+"/data");
	  qint64 n=4000;
	  n=n*1024*1024;
     QWebSettings::globalSettings()->setOfflineStorageDefaultQuota (n);
	 connect(this,SIGNAL(frameCreated(QWebFrame*)),this,SLOT(frameCreatedSlot(QWebFrame*)));
	 connect(mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(populateJavaScriptWindowObject()));
	if(!m_utl)
		m_utl=new WebUtils(this); 
}

void AutoWebPage::javaScriptConsoleMessage(const QString &message, int lineNumber,
                                       const QString &sourceID)
{
	QString s=sourceID.mid(sourceID.lastIndexOf('/'));
	qWarning()<<s<<":"<<lineNumber<<":"<<message;
}

QWebPage *AutoWebPage::createWindow(QWebPage::WebWindowType type)
{
    Q_UNUSED(type);
   qDebug()<<"want new window";
   return this;
}
/**
  通过指定的参数来创建页面内容,主要通过js来管理页面
<object type='application/x-qt-plugin' classid='lineedit' id='myedit'/>
可以设置代理,cookie,cache等信息, 根据观察=号左侧传递进来的全部都是小写
参数列表:
代理:
proxyAddress="202.113.25.68" proxyType="http|socks5"
proxyPort="80" proxyUser="test" proxyPassword="123"

cookieType="session|permanence"
cache="nocache"

filter="*.jpg|*.png|*.gif"
  */
QObject *  AutoWebPage::createPlugin(const QString &classId,
                                      const QUrl &,
                                      const QStringList &paramNames,
                                      const QStringList &paramValues)
{
    qDebug()<<"classId:"<<classId;
    qDebug()<<paramNames;
    qDebug()<<paramValues;

    QString name="webview";
    if(classId!="webview")
        return NULL;
    AutoWebView *  view=new AutoWebView();
    QUrl url;
    QNetworkProxy proxy(QNetworkProxy::NoProxy);
    QString session="newthread";
	QString sessionId=""; // 创建的object的id,也就是这个session的sessionId
    int i=0;
    if((i=paramNames.indexOf("url"))!=-1)
	{
        url=paramValues.at(i);
		if(url.isRelative())
		{
			url=mainFrame()->url();
			QString path=mainFrame()->url().path();
			path=path.left(path.lastIndexOf('/'));
			path+="/"+paramValues.at(i);;
			url.setPath(path);
			qDebug()<<url;
		}
	}
	if((i=paramNames.indexOf("id"))!=-1)
	{
		sessionId=paramValues.at(i);
		view->setProperty("id",sessionId);
	}
	else
		goto createWebViewFailure;
	qDebug()<<"load:"<<url;
    view->load(url);
    //删除相应的资源,以后如果做的好的话,webview对象应该可以重复利用才对
    connect(view,SIGNAL(destroyed()),this,SLOT(webDestroyed()));
    qDebug()<<"view:"<<view;
    return view;
createWebViewFailure:
	QString s="paramNames:";
	s+=paramNames.join(",");
	s+="\n";
	s+="paramValues:";
	s+=paramValues.join(",");
	QMessageBox::about(this->view(),"创建webview失败",s);
	if(view)
		view->deleteLater();
	return NULL;
}
void AutoWebPage::openPage( const QString & name,QMap<QString ,QVariant> args )
{
	Q_UNUSED(name);
	Q_UNUSED(args);
	foreach(QString key,args.keys())
	{
		//getUtil()->put(key,args.value(key));
	}
	//view()->load(getSetting()->getStringValue("htmlfiledir")+name);
}
void AutoWebPage::webDestroyed()
{
	qDebug()<<"AutoWebPage::webDestroyed";
}

QString AutoWebPage::userAgentForUrl(const QUrl& url)
{
	Q_UNUSED(url);
	return "Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.0.8) Gecko/2009032609 Firefox/3.6.8; QT ";
}

void AutoWebPage::populateJavaScriptWindowObject()
{
	qDebug()<<"javascript clear !!";
	qDebug()<<"populateJavaScriptWindowObject";
	QWebFrame * frame=qobject_cast<QWebFrame*>(sender());
	qDebug()<<"frame is null:"<<(frame==NULL);
	if(frame)
		frame->addToJavaScriptWindowObject("webutil",m_utl);
}

void AutoWebPage::frameCreatedSlot( QWebFrame * frame )
{
	qDebug()<<"frame create!!";
	connect(frame,SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(populateJavaScriptWindowObject()));
}

void AutoWebPage::javaScriptAlert( QWebFrame *originatingFrame, const QString& msg )
{
	QMessageBox::information(this->view(),"",msg,QMessageBox::Ok);
}



AutoWebView::AutoWebView(QWidget *parent)
    :QWebView(parent)
{
    this->setPage(new AutoWebPage(this));
//    page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal,Qt::ScrollBarAlwaysOn);
//  page()->mainFrame()->setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOn);

  connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
          this, SLOT(populateJavaScriptWindowObject()));
  connect(page(),SIGNAL(loadFinished(bool)),this,SLOT(loadFinished(bool)));
  connect(page(),SIGNAL(loadStarted()),this,SLOT(loadStarted()));
  connect(page(),SIGNAL(loadProgress(int)),this,SIGNAL(loadProgress(int)));


}

void AutoWebView::populateJavaScriptWindowObject()
{
    
}



void AutoWebView::loadFinished(bool ok)
{
    qDebug()<<ok<<" load finished:"<<this->url();
    if(!ok)
    {
        qDebug()<<"load error:"<<this->url();
        return ;
    }
   // runOnload();
}
void AutoWebView::loadStarted()
{
    qDebug()<<"load Started:"<<url();
}
void AutoWebView::loadProgress(int progress)
{
    qDebug()<<"progress:"<<progress;
}
void AutoWebView::print(const QString &s)
{
	qDebug()<<s;
}
/**
  测试通过javascript获取QObject对象,是否已经转换为js的Object对象
  */
QWebPage *  AutoWebView::webpage()
{
    return this->page();
}

void AutoWebView::contextMenuEvent(QContextMenuEvent * event)
{
	QWebView::contextMenuEvent(event);
	return;
    QMenu menu(this);
    //menu.addAction(tr("Restart Session"),this->m_userData, SLOT(restartSession()));
    menu.addAction(pageAction(QWebPage::CopyLinkToClipboard));
    if (page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
        menu.addAction(pageAction(QWebPage::InspectElement));
	menu.addAction(pageAction(QWebPage::Reload));
	menu.addAction(pageAction(QWebPage::Back));
	menu.addAction(pageAction(QWebPage::Forward));
    menu.exec(mapToGlobal(event->pos()));
}
void AutoWebView::mousePressEvent(QMouseEvent * e)
{
    return  QWebView::mousePressEvent(e);
    qDebug()<<"Pos:"<<e->pos();
    qDebug()<<"globalpos:"<<e->globalPos();
    qDebug()<<"this:"<<this->pos();
    qDebug()<<this->geometry();
    //this->resize();
    QWebView::mousePressEvent(e);

}



