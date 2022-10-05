/***************************************************************************
                          popupwindow.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004-2012
    copyright            : (C) 2004 by Alain Gibaud
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

#include <QLabel>

#include "PopupWindow.h"


PopupWindow::PopupWindow (QWidget *parent, const QString& txt) : QMenu (parent)
{
    QLabel *lab = new QLabel (txt, this);
    setView (lab);
    lab->setBackgroundColor (QColor (0xFFFFDE)); // tooltip yellow
    setBackgroundColor (QColor (0xFFFFDE));
    setLineWidth (1);
    show ();
}

PopupWindow::~PopupWindow ()
{
}
