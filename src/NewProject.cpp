/***************************************************************************
                        NewProject.cpp  -  description
                             -------------------
    begin                : Fri Nov 28 2003
    copyright            : (C) 2003 by Alain Gibaud
    email                : alain.gibaud@univ-valenciennes.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
using namespace std;

#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>

#include "FNU.h"
#include "NewProject.h"
#include "PikMain.h"


NewProject::NewProject () : QMainWindow (0)
{
    setAttribute (Qt::WA_DeleteOnClose);

    QIcon iconset (":/src/icons/22/pikdev_openfile.png");
    
    setWindowTitle (tr ("New project"));
    
    QWidget *central = new QWidget (this);
    setCentralWidget (central);
    QGridLayout *Lcentral = new QGridLayout (central);

    // start of the groupbox
    QGroupBox *vgb = new QGroupBox (tr ("Project description"), central);
    QGridLayout *Lvgb = new QGridLayout (vgb);

    Lcentral->addWidget (vgb, 0, 0, 1, 3);

    // line 1
    Lvgb->addWidget (new QLabel (tr ("Name"), central), 0, 0);
    project_name = new QLineEdit (central);
    Lvgb->addWidget (project_name, 0, 1);
    
    // line 2
    Lvgb->addWidget (new QLabel (tr ("Directory"), central), 1, 0);
    project_dir = new QLineEdit (central);
    Lvgb->addWidget (project_dir, 1, 1);
    QPushButton *bdir = new QPushButton (iconset, "", central);
    connect (bdir, SIGNAL (clicked ()), this, SLOT (slotSelectDir ()));
    Lvgb->addWidget (bdir, 1, 2);

    // line 3
    Lvgb->addWidget (new QLabel (tr ("Type"), central), 2, 0);
    project_type = new QComboBox (central);
    project_type->setEditable (false);
    project_type->addItem (tr ("Asm"));
    project_type->addItem (tr ("Cpik for p18xxx"));
    Lvgb->addWidget (project_type, 2, 1);

    // line 4
    Lvgb->addWidget (new QLabel (tr ("Configure now"), central), 3, 0);
    config_now = new QCheckBox (central);
    config_now->setChecked (true);
    Lvgb->addWidget (config_now, 3, 1);
    // end of groupbox

    QPushButton *create = new QPushButton (tr ("Create"), central);
    connect (create, SIGNAL (clicked ()), this, SLOT (slotCreate ()));
    Lcentral->addWidget (create, 1, 1);
    
    QPushButton *cancel = new QPushButton (tr ("Cancel"), central);
    connect (cancel, SIGNAL (clicked ()), this, SLOT (slotCancel ()));
    Lcentral->addWidget (cancel, 1, 2);

    setMinimumWidth (500);
    show ();
}

NewProject::~NewProject ()
{
}

/* This slot must be activated from the browse button and update
   the project_dir lineedit. */
void NewProject::slotSelectDir ()
{
    QString dir = QFileDialog::getExistingDirectory (this, tr ("Select project directory"),
						     "", QFileDialog::ShowDirsOnly);

    if (! dir.isEmpty ())
      project_dir->setText (dir);
}

void NewProject::slotCancel ()
{
    close ();
}

void NewProject::slotCreate ()
{
    if (createProjectEntry () && config_now->isChecked ())
      {
        // project entry created, do more configuration.
        if (PikMain::mainWindow ())
	  PikMain::mainWindow ()->projectWid ()->slotEditProject ();
      }
    slotCancel ();
}

bool NewProject::createProjectEntry ()
{
    if (project_name->text () == "" || project_dir->text () == "")
      {
        QMessageBox::warning (this, tr ("Sorry"),
			      tr ("<b>Directory</b> and <b>Project name</b>"
                                  "  must be provided"),
			      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);


        return false;
      }

    QString type;
    int t = project_type->currentIndex ();

    if (t == 0)
      type = "asm";
    else if (t == 1)
      type = "C";
    else
      return false;

    emit projectData (project_name->text (), project_dir->text (), type);

    return true;
}
