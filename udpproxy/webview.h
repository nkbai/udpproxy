#pragma  once
#include <QtWebKit>
#include "webutils.h"

class AutoWebPage : public QWebPage
{
	Q_OBJECT
public:
	AutoWebPage(QObject * parent=0);
	virtual void	javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID );
	QWebPage *createWindow(QWebPage::WebWindowType type);
	virtual void javaScriptAlert(QWebFrame *originatingFrame, const QString& msg);
	//virtual QString chooseFile(QWebFrame *originatingFrame, const QString& oldFile);
	//virtual bool javaScriptConfirm(QWebFrame *originatingFrame, const QString& msg);
	//virtual bool javaScriptPrompt(QWebFrame *originatingFrame, const QString& msg, const QString& defaultValue, QString* result);
	virtual QString userAgentForUrl(const QUrl& url);
	virtual QObject * createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
	public slots:
		void populateJavaScriptWindowObject();
		void frameCreatedSlot ( QWebFrame * frame );
		void openPage(const QString & name,QMap<QString ,QVariant> args);
	public slots:
		void webDestroyed();
public:
	WebUtils * getUtil()
	{
		return m_utl;
	}
private:
	WebUtils * m_utl;
};

class AutoWebView : public QWebView
{
	Q_OBJECT

public:
	AutoWebView(QWidget *parent=0);
protected:
	virtual void contextMenuEvent(QContextMenuEvent *);
	virtual void mousePressEvent(QMouseEvent *);
public slots:
	void populateJavaScriptWindowObject();
	void loadFinished(bool ok);
	void	loadProgress ( int progress );
	void	loadStarted ();
	QWebPage * webpage();

public slots:
	void print(const QString & s);
	

private:
	
};
