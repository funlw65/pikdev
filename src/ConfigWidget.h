/***************************************************************************
                        ConfigWidget.h  -  description
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
/* Base class for configuration widgets
   @author Gibaud Alain */

#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QLayout>
#include <QPixmap>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>


class ConfigWidget : public QWidget
{
    Q_OBJECT
    QVBoxLayout *mylayout;

public: 
    ConfigWidget (QWidget *parent = 0, const char *name = 0);
    virtual ~ConfigWidget ();
    void addWidget (QWidget *w);

public slots:
    virtual void doApply () = 0;
  
    // Return the name of the config widget
    virtual QString configName () = 0;
  
    // Return the title of the config page
    virtual QString configTitle () = 0;
  
    virtual QPixmap icon () = 0;
};

#endif
