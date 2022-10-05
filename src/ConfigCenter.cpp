/***************************************************************************
                       ConfigCenter.cpp  -  description
                             -------------------
    begin                : Wed Oct 13 2004
    copyright            : (C) 2004 by Gibaud Alain
    email                : alain.gibaud@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QColor>
#include <QFont>

#include "AsmConfigWidget.h"
#include "ConfigCenter.h"
#include "EditorConfigWidget.h"
#include "GlobalConfigWidget.h"
#include "PrgConfigWidget.h"


ConfigCenter::ConfigCenter (PikMain *main) : QMainWindow (0)
{
    setAttribute (Qt::WA_DeleteOnClose);

    // splitter
    splitv =  new QSplitter (this);
    splitv->setHandleWidth (5);
    splitv->setOrientation (Qt::Horizontal);
    splitv->setOpaqueResize (true);

    setCentralWidget(splitv);
    // add control tree
    QWidget *treebox = new QWidget (splitv);
    splitv->addWidget (treebox);
    QVBoxLayout *Ltreebox = new QVBoxLayout (treebox);

    confView = new QListWidget (treebox);
    confView->setSpacing (4);
    confView->setIconSize (QSize (32, 32));
    Ltreebox->addWidget (confView);

    treebox->setMinimumWidth (165);

    connect (confView, SIGNAL (currentItemChanged (QListWidgetItem *, QListWidgetItem *)),
             this, SLOT (raiseConfigWidget (QListWidgetItem *)));

    // right side
    QWidget *vb = new QWidget (splitv);
    splitv->addWidget (vb);

    QGridLayout *Lvb = new QGridLayout (vb);

    // title, with icon in the corner HG
    image = new QLabel ("", vb);
    Lvb->addWidget (image, 0, 0);

    title = new QLabel ("", vb);
    Lvb->addWidget (title, 0, 1);

    Lvb->setColumnStretch (0, 1);
    Lvb->setColumnStretch (1, 10);

    title->setAlignment (Qt::AlignCenter);
    QFont f = title->font ();
    f.setPointSize (14);
    title->setFont (f);

    // configuration widget stack
    wstack = new QStackedWidget (vb);
    Lvb->addWidget (wstack, 1, 0, 1, 2);

    // Buttons
    QHBoxLayout *Lbuttons = new QHBoxLayout;
    Lbuttons->setSpacing (10);
    Lvb->addLayout (Lbuttons, 2, 0, 1, 2);

    QWidget *space = new QWidget (vb);
    Lbuttons->addWidget (space);
    okButton = new QPushButton (tr ("Ok"), vb);
    Lbuttons->addWidget (okButton);
    applyButton = new QPushButton (tr ("Apply"), vb);
    Lbuttons->addWidget (applyButton);
    cancelButton = new QPushButton (tr ("Quit"), vb);
    Lbuttons->addWidget (cancelButton);
    Lbuttons->setStretchFactor (space, 3);
    Lbuttons->setStretchFactor (cancelButton, 1);
    Lbuttons->setStretchFactor (applyButton, 1);
    Lbuttons->setStretchFactor (okButton, 1);

    connect (cancelButton, SIGNAL (clicked ()), this, SLOT (slotClose ()));
    connect (okButton, SIGNAL (clicked ()), this, SLOT (slotOk ()));
    connect (applyButton, SIGNAL (clicked ()), this, SLOT (slotApply ()));

    // plug programmer configurators
    QVector<ProgrammerBase *>::iterator
      cf = main->programmersList ().begin (), cfend = main->programmersList ().end ();

    for (; cf != cfend; ++cf)
      {
        ConfigWidget *cw = (*cf)->configWidget ();
        // some programmers doesn't need configurator
        if (cw)
	  plugConfigWidget (cw);
      }

    plugConfigWidget (new EditorConfigWidget (main));
    plugConfigWidget (new AsmConfigWidget);
    plugConfigWidget (new GlobalConfigWidget (main));

    if (confView->count ())
      {
        QListWidgetItem *first = confView->item (0);
        raiseConfigWidget (first);
      }

    setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);
    show ();
}

ConfigCenter::~ConfigCenter ()
{
}

// Insert a new config widget
void ConfigCenter::plugConfigWidget (ConfigWidget *cw)
{
    int id = wstack->addWidget (cw) ;

    // user id(s) begin at number 1000
    __attribute__ ((unused)) QListWidgetItem *conf
      = new QListWidgetItem (cw->icon (), cw->configName (),
			     confView, id + 1000);

    title->setText (cw->configTitle ());
    image->setPixmap (cw->icon ());
}

void ConfigCenter::slotClose ()
{
    close ();
}

void ConfigCenter::slotOk ()
{
    if(wstack->currentWidget ())
      {
	ConfigWidget *ac = dynamic_cast<ConfigWidget *>(wstack->currentWidget ());
        if (ac)
	  ac->doApply ();
        slotClose ();
      }
}

void ConfigCenter::slotApply ()
{
    if (wstack->currentWidget ())
      {
        ConfigWidget *ac = dynamic_cast<ConfigWidget *>(wstack->currentWidget ());
        if (ac)
	  ac->doApply ();
      }
}

void ConfigCenter::raiseConfigWidget (QListWidgetItem *item)
{
    if (item == 0)
      return;

    int id = item->type () - 1000;
    item->setSelected (true);
    confView->setCurrentItem (item);

    QWidget *w = wstack->widget (id);
    wstack->setCurrentWidget (w);
    ConfigWidget *ac = dynamic_cast<ConfigWidget *>(w);
    title->setText (ac->configTitle ());
    image->setPixmap (ac->icon ());
}
