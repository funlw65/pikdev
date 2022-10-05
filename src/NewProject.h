/***************************************************************************
                         NewProject.h  -  description
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
/* Widget for specifying informations about a new project
   @author Alain Gibaud */

#ifndef NEWPROJECT_H
#define NEWPROJECT_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QString>


class NewProject : public QMainWindow
{
    Q_OBJECT

public: 
    NewProject ();
    ~NewProject ();
    bool createProjectEntry ();

public:

private:
    QLineEdit *project_name, *project_dir;
    QComboBox *project_type;
    QCheckBox *config_now;

private slots:
    // This slot must be activated from the browse button and update the project_dir string.
    void slotSelectDir ();
    // void slotSelectOut ();
    void slotCancel ();
    // Collect project data and emit the projectData signal
    void slotCreate ();
    // void slotNameChanged (const QString&);

signals:
    void projectData (const QString& name, const QString& dir, const QString& type);
};

#endif
