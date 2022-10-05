#include <cstdio>
#include <iostream>
using namespace std;

#include <QApplication>
#include <QFrame>
#include <QPixmap>
#include <QPushButton>
#include <QToolBar>
#include <QToolTip>
#include <QVBoxLayout>

#include "ConfigBits.h"
#include "ConfigBitsWizard.h"
#include "ConfigWordField.h"
#include "DraggableLabel.h"
#include "SimplePopup.h"
#include "configw.xpm.h"
#include "configwH.xpm.h"
#include "configwHL.xpm.h"
#include "configwL.xpm.h"
#include "pic.hh"


ConfigBitsWizard::ConfigBitsWizard (ProjectWid *parent)
    : QMainWindow (parent), deviceData (0),
      current_project (0), config (0)
{
    setAttribute (Qt::WA_DeleteOnClose);
    // get project & pic device
    current_project = PikMain::mainWindow ()->projectWid ()->getProject ();
    QString devname = current_project->getDevice ();
    pikDB *db = pikDB::pikdb ();
    QByteArray baDevname = devname.toLatin1 ();
    deviceData = db->getData (baDevname);

    setWindowTitle (tr ("Config bits editor and config code generator (%1)").
		    arg (deviceData->gpName));
    QFrame *vb = new QFrame (this);
    QVBoxLayout *vbl = new QVBoxLayout (vb);  /* , 8, 8  TODO */
    vb->setLineWidth (1);
    setCentralWidget (vb);
    confView = new QTreeWidget (vb);
    vbl->addWidget (confView);
    confView->setColumnCount (3);
    confView->setColumnWidth (0, 200);
    confView->setColumnWidth (1, 270);
    confView->setColumnWidth (2, 400);
    confView->setRootIsDecorated (true);
    QStringList lh;
    lh << tr ("Config Words/Fields") << tr ("Role") << tr ("Current state");
    confView->setHeaderLabels (lh);

    // get list of config registers
    config = new ConfigBits (deviceData);

    // get config register from project
    slotReadConfig ();
    QVector<ConfigBitsWord>& thewords = config->theWords ();

    for (int i = thewords.size () - 1; i >= 0; --i)
      {
        QString vals = "[";
        vals += thewords[i].binaryString ();
        vals += "] [";
        vals += thewords[i].hexString ();
        vals += "]";

        QStringList wl;
        wl << thewords[i].name () << "" << vals;

        QTreeWidgetItem *wordItem = new QTreeWidgetItem (confView, wl);
        wordItem->setIcon (0, QIcon (QPixmap (configwHL)));
        confView->addTopLevelItem (wordItem);
        wordItem->setExpanded (true);

        QVector<ConfigWordField>& thefields = thewords[i].theFields ();

        for (int j = 0; j < (int) thefields.size (); ++j)
	  {
            QString name  = thefields[j].nameAndSize ();
            QString desc = thefields[j].description ();
            QString valuedesc;

            int index = thefields[j].getValueIndex ();
            int mini = thefields[j].getMini ();
            if (index != -1)
	      valuedesc = thefields[j].legalValueDesc (index);

            if (name[0] != '0' && name[0] != '1')
	      {
                wl.clear ();
                wl << name << desc << valuedesc
		   << QString::asprintf ("%d %d %d", i, j, index);

                QTreeWidgetItem *fieldItem = new QTreeWidgetItem (wordItem, wl);

                const char **ico;

                if (deviceData->core != 16)
		  ico = configw;
                else
                  ico =	(mini > 7) ? configwH : configwL;

                fieldItem->setIcon (0, QIcon (QPixmap (ico)));
	      }
	  }
      }

    setContextMenuPolicy (Qt::CustomContextMenu);
    connect (this, SIGNAL (customContextMenuRequested (const QPoint&)),
             this, SLOT (slotContextMenu (const QPoint&)));

    // make toobar 1 - this is not a classic toolbar,
    // because it contains draggable icons
    QToolBar *bar1 = new QToolBar (this);
    addToolBar (bar1);
    // to get icons ...

    // first draggable button
    DraggableLabel *dragCbutton = new DraggableLabel (bar1, "", config, 1);
    dragCbutton->setToolTip (tr ("Drag this icon to your C source code to get the current CONFIG bits definitions"));

    QPixmap dragC (":/src/icons/22/pikdev_dragC.png");
    dragCbutton->setPixmap (dragC);
    bar1->addWidget (dragCbutton);
    QWidget *sep1 = new QWidget(bar1);
    sep1->setFixedWidth (6);
    bar1->addWidget (sep1);

    // second draggable button
    DraggableLabel *dragAbutton = new DraggableLabel (bar1, "", config, 2);
    dragAbutton->setToolTip (tr ("Drag this icon to your ASM source code to get the current CONFIG bits definitions"));
    QPixmap dragA (":/src/icons/22/pikdev_dragA.png");
    dragAbutton->setPixmap (dragA);
    bar1->addWidget (dragAbutton);
    QWidget *sep2 = new QWidget (bar1);
    sep2->setFixedWidth (6);
    bar1->addWidget (sep2);

    // second toolbar for file generation and exit
    QToolBar *bar2 = new QToolBar (this);

    QPixmap make_headers (":/src/icons/22/pikdev_make_headers.png");

    QAction *mh = new QAction (QIcon (make_headers), tr ("&Make header files"), this);
    mh->setShortcut (QKeySequence ("Ctrl+H"));

    bar2->addAction (mh);
    connect (mh, SIGNAL (activated ()), this, SLOT (slotMakeCode ()));

    QPixmap file_close (":/src/icons/22/pikdev_close.png");

    QAction *close = new QAction (QIcon (file_close), tr ("&Quit editor"), this);
    close->setShortcut (QKeySequence ("Ctrl+Q"));

    bar2->addAction (close);
    connect (close, SIGNAL (activated ()), this, SLOT (slotClose ()));

    addToolBar (bar2);

    // menu
    QMenu *menu = menuBar ()->addMenu (tr ("Actions"));
    menu->addAction (mh);
    menu->addAction (close);

    statusBar ()->showMessage (tr ("  Current device is %1  ").arg (deviceData->gpName), 10000);
    setWindowIcon (QPixmap (":/src/icons/16/pikdev_icon.png"));
    setMinimumSize (QSize (910, 500));
    show ();
}

ConfigBitsWizard::~ConfigBitsWizard ()
{
    delete config;
}

void ConfigBitsWizard::slotContextMenu (const QPoint& pglob)
{
    // transform p to local widget coordinates
    QPoint p (pglob.x () + x (), pglob.y () + y ());

    QList<QTreeWidgetItem *> items = confView->selectedItems ();
    if (items.size () == 1)
      {
        QTreeWidgetItem *item = items[0];
        if (item->text (3) != "")
	  {
            // decode config word and field numbers
            int wn, fn, value_index;
            QByteArray ba3 = item->text (3).toLatin1 ();
	    
            if (sscanf (ba3, "%d %d %d", &wn, &fn, &value_index) != 3)
	      return;
	    
            QVector<ConfigBitsWord>& thewords = config->theWords ();
            QVector<ConfigWordField>& thefields = thewords[wn].theFields ();

            SimplePopup *pop = new SimplePopup (tr ("Select a configuration"), this);

            for (int i = 0; i < thefields[fn].legalValuesCount (); ++i)
	      pop->addItem (thefields[fn].legalValueDesc (i), i);

            int menu_index = pop->exec (p);

            if (menu_index >= 0)
	      {
                thefields[fn].setValue (menu_index);
                item->setText (2, thefields[fn].legalValueDesc (menu_index));
                const QString s = QString::asprintf ("%d %d %d", wn, fn, menu_index);
                item->setText (3, s);

                // now update config object
                thefields[fn].setValue (menu_index);

                // and save new config to project file
                slotWriteConfig ();

                // update config word display
                QTreeWidgetItem *parent = item->parent ();
                QString vals = "[";
                vals += thewords[wn].binaryString ();
                vals += "] [";
                vals += thewords[wn].hexString ();
                vals += "]";
                parent->setText (2, vals);
	      }
	  }
      }
}

void ConfigBitsWizard::slotWriteConfig ()
{
    current_project->setConfigWords (config->getRawValues ());
}

void ConfigBitsWizard::slotReadConfig ()
{
    // read raw config bits values from project
    QVector<int> values = current_project->getConfigWords ();

    // set the ConfigBits object accordingly
    const char *message;
    if ((message = config->setRawValues (values)) != 0)
      statusBar ()->showMessage (QString (message), 10000);
}

void ConfigBitsWizard::slotMakeCode ()
{
    QString dir = current_project->projectPath ();
    config->writeCCode (dir);
    config->writeASMCode (dir);
    statusBar ()->showMessage (tr (" [config_bits.h] and [config_bits.inc] created  in %1 ")
			       .arg (dir), 10000);
}

void ConfigBitsWizard::slotClose ()
{
    emit closed ();
    close ();
}

bool ConfigBitsWizard::queryClose ()
{
    emit closed ();
    return true;
}

void ConfigBitsWizard::closeEvent (QCloseEvent *event)
{
    emit closed ();
    event->accept ();
}
