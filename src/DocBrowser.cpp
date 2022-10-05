#include <iostream>
using namespace std;

#include <QCloseEvent>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QToolBar>

#include "DocBrowser.h"


DocBrowser::DocBrowser (const QString& page) : QMainWindow (0)
{
    setAttribute (Qt::WA_DeleteOnClose);

    actions.addAction ("previous", ":/src/icons/22/go-previous.png",
		       "Previous page", QKeySequence (), this);
    actions.addAction ("next", ":/src/icons/22/go-next.png",
		       "Next page", QKeySequence (), this);
    actions.addAction ("close", ":/src/icons/22/pikdev_close.png",
		       "Close browser", QKeySequence (), this);

    QToolBar *bar = new QToolBar(this);
    bar->setObjectName ("Navigation");
    bar->addAction (actions.act ("previous"));
    bar->addAction (actions.act ("next"));
    bar->addAction (actions.act ("close"));
    addToolBar (bar);

    actions.addAction ("cpik-doc", "", "CPIK Compiler", QKeySequence (), this);
    actions.addAction ("devices-doc", "", "PIC devices data", QKeySequence (), this);
    QToolBar *docbar = new QToolBar (this);
    docbar->setObjectName ("Documentation chooser");
    docbar->addSeparator ();
    docbar->addAction (actions.act ("cpik-doc"));
    docbar->addSeparator ();
    docbar->addAction (actions.act ("devices-doc"));
    addToolBar (docbar);
    docbar->addSeparator ();

    browser = new QWebView (this);
    connect (actions.act ("previous"), SIGNAL (triggered ()), browser, SLOT (back ()));
    connect (actions.act ("next"), SIGNAL (triggered ()), browser, SLOT (forward ()));
    connect (actions.act ("close"), SIGNAL (triggered ()), this, SLOT (slotClose ()));

    connect (browser, SIGNAL (urlChanged (QUrl)), this, SLOT (slotUrlChanged (QUrl)));

    connect (actions.act ("devices-doc"), SIGNAL (triggered ()), this, SLOT (slotDevicesDoc ()));
    connect (actions.act ("cpik-doc"), SIGNAL (triggered ()), this, SLOT (slotCpikDoc ()));

    if (page != "")
      browser->load (currentpage = page);
    else
      browser->setHtml (tr ("<hr><center><b>Select the documentation you need above</b></center><hr>"));

    browser->show ();
    setCentralWidget (browser);
    setMinimumWidth (1000);
    setWindowIcon (QPixmap (":/src/icons/16/pikdev_icon.png"));
    show ();
}

void DocBrowser::slotUrlChanged (const QUrl& url)
{
    QString txt = url.toString ();
    // stripout path ?
    setWindowTitle (txt);
}

bool DocBrowser::searchDevicesDoc ()
{
    QString basepath = "/usr/local/share/doc/";
    currentpage = "";

    // search for a gputils* directory

    QDir dir (basepath);
    QStringList filters;
    filters << "gputils*";
    QStringList versions = dir.entryList (filters, QDir::Dirs, QDir::Name | QDir::Reversed);

    if (! versions.isEmpty ())
      {
        // the first name corresponds to the most recent version
        QString path = basepath + versions.front () + "/html/";
        QFile f;
        QString candidate = path + "index.html";

        f.setFileName (candidate);
        if (f.exists ())
	  {
            currentpage = candidate;
            return true;
	  }
      }
    return false;
}

void DocBrowser::closeEvent (QCloseEvent *event)
{
    emit browserClosed (browser->url ().toString ());
    event->accept ();
}

bool DocBrowser::searchCpikDoc ()
{
    QString basepath = "/usr/share/cpik/";
    currentpage = "";

    // search for a gputils* directory

    QDir dir (basepath);
    QStringList filters;
    filters << "*.*.*";
    QStringList versions = dir.entryList (filters, QDir::Dirs, QDir::Name | QDir::Reversed);

    if (! versions.isEmpty ())
      {
        // the first name corresponds to the most recent version
        QString path = basepath + versions.front () + "/doc/html/";
        QFile f;
        QString candidate = path + "index.html";

        f.setFileName (candidate);
        if (f.exists ())
	  {
            currentpage = candidate;
            return true;
	  }
      }
    return false;
}

void DocBrowser::slotDevicesDoc ()
{
    searchDevicesDoc ();
    if (currentpage != "")
      browser->load (currentpage);
    else
      browser->setHtml (tr ("Sorry.. The devices documentation cannot be found.<br>"
                            "Please install the gputils tools (version >= 1.0.0)<br>"
                            "<a href=\"http://gputils.sourceforge.net/\">(http://gputils.sourceforge.net/)</a>"));
}

void DocBrowser::slotCpikDoc ()
{
    searchCpikDoc ();
    if (currentpage != "")
      browser->load (currentpage);
    else
      browser->setHtml (tr ("Sorry.. The documentation for cpik cannot be found.<br>"
                            "Please install cpik (version >= 0.7.2)<br>"
                            "<a href=\"http://pikdev.free.fr/\">(http://pikdev.free.fr/)</a>"));
}

void DocBrowser::slotClose ()
{
    close ();
}
