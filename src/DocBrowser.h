#ifndef DOCBROWSER_H
#define DOCBROWSER_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QUrl>
#include <QWebView>

#include "ActionsCollection.h"


class DocBrowser : public QMainWindow
{
    Q_OBJECT
    QWebView *browser;
    ActionsCollection actions;
    QString currentpage;

public:
    DocBrowser (const QString& page);
    bool searchDevicesDoc ();
    bool searchCpikDoc ();
    void closeEvent (QCloseEvent *event);

public slots:
    void slotUrlChanged (const QUrl& url);
    void slotDevicesDoc ();
    void slotCpikDoc ();
    void slotClose ();

signals:
    void browserClosed (const QString&);
};

#endif // DOCBROWSER_H
