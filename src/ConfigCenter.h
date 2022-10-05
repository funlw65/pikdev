/***************************************************************************
                        ConfigCenter.h  -  description
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
/* Tabbed organizer for ConfigWidgets
   @author Gibaud Alain */

#ifndef CONFIGCENTER_H
#define CONFIGCENTER_H

#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QToolBox>

#include "ConfigWidget.h"
#include "PikMain.h"


class ConfigWidget;
class PikMain;
class ConfigCenter : public QMainWindow
{
   Q_OBJECT

public: 
    ConfigCenter (PikMain *main);
    ~ConfigCenter ();
    // Insert a new config widget
    void plugConfigWidget (ConfigWidget *c);

private:
    QLabel *title, *image;
    QPushButton *applyButton, *cancelButton, *okButton;
    QListWidget *confView;
    // Configurator widgets stack
    QStackedWidget *wstack;
    QSplitter *splitv ;
		    
public slots:
    virtual void slotOk ();
    virtual void slotClose ();
    virtual void slotApply ();
			   
public slots:
    void raiseConfigWidget (QListWidgetItem *item);
};

#endif
