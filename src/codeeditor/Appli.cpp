#include <iostream>
using namespace std;

#include <QtGui>

#include "Appli.h"
#include "ItemSelector.h"
#include "QBEditor.h"


Appli::Appli () : QMainWindow (0)
{
    edit = new QBEditor (this);
    setCentralWidget (edit);

    QMenu *menuFile = new QMenu ("File");
    menuBar ()->addMenu (menuFile);

    QMenu *menuEdit = new QMenu ("Edit");
    menuBar ()->addMenu (menuEdit);

    QMenu *dynamicBookmarks = new QMenu ("Bookmarks");
    menuBar ()->addMenu (dynamicBookmarks);

    QMenu *menuDevel = new QMenu ("Development");
    menuBar ()->addMenu (menuDevel);
    QAction *experiment = new QAction ("Dialog", this);
    menuDevel->addAction (experiment);
    connect (experiment, SIGNAL (triggered ()), this, SLOT (slotExperiment ()));

    QAction *quit = new QAction (QIcon (), tr ("&Quit"), this);


    menuFile->addAction (edit->act ("new"));
    menuFile->addAction (edit->act ("open"));
    menuFile->addAction (edit->act ("save"));
    menuFile->addAction (edit->act ("save-as"));
    menuFile->addAction (edit->act ("close"));
    menuFile->addAction (quit);

    menuEdit->addAction (edit->act ("undo"));
    menuEdit->addAction (edit->act ("redo"));
    menuEdit->addAction (edit->act ("cut"));
    menuEdit->addAction (edit->act ("copy"));
    menuEdit->addAction (edit->act ("paste"));

    menuEdit->addAction (edit->act ("search-replace"));
    menuEdit->addAction (edit->act ("goto"));

    menuEdit->addSeparator ();
    menuEdit->addAction (edit->act ("line-numbers"));
    menuEdit->addAction (edit->act ("bookmarks"));

    menuEdit->addSeparator ();
    menuEdit->addAction (edit->act ("comment"));
    menuEdit->addAction (edit->act ("uncomment"));

    connect (dynamicBookmarks, SIGNAL (aboutToShow ()), edit, SLOT (slotShowBookmarksMenu ()));
    connect (quit, SIGNAL (activated ()), this, SLOT (slotQuit ()));

    setMinimumSize (QSize (640, 480));
}

void Appli::slotQuit ()
{
    edit->slotAboutToClose ();
    qApp->quit ();
}

void Appli::slotExperiment ()
{
}
