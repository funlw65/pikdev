/***************************************************************************
                        ConfigWidget.cpp  -  description
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

#include "ConfigWidget.h"


ConfigWidget::ConfigWidget (QWidget *parent, __attribute__ ((unused)) const char *name)
    : QWidget (parent)
{
    mylayout = new QVBoxLayout (this);
}

void ConfigWidget::addWidget (QWidget *w)
{
    mylayout->addWidget (w);
}

ConfigWidget::~ConfigWidget ()
{
}

void ConfigWidget::doApply ()
{
}
