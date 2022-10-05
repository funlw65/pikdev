/***************************************************************************
                       GlobalConfigWidget.h  -  description
                             -------------------
    begin                : Thu Feb 2 2006
    copyright            : (C) 2006 by Gibaud Alain
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
/* @author Gibaud Alain */

#ifndef GLOBALCONFIGWIDGET_H
#define GLOBALCONFIGWIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QVector>

#include "ConfigWidget.h"
#include "PikMain.h"


class GlobalConfigWidget : public ConfigWidget
{
    Q_OBJECT

    QComboBox *programmers; // programmers list
    QVector<QLabel *> tool_names;
    QVector<QLineEdit *> tool_paths;
    // the app widget
    PikMain *main_widget;

public:
    GlobalConfigWidget (PikMain *pikmain);
    ~GlobalConfigWidget ();

    // short name of configurator
    virtual QString configName ();
    // title string for this configurator
    virtual QString configTitle ();
    // return icon of this configurator
    virtual QPixmap icon ();

public slots:
    // apply configured data (i.e.: save them to config file)
    virtual void doApply ();

private slots:
    void chooseExecutable (int i);
};

#endif
