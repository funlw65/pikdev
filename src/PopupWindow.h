/***************************************************************************
                          popupwindow.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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
/* A yellow popup window for temporarily displaying text information
   @author Alain Gibaud */

#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QMenu>
#include <QWidget>


class PopupWindow : public QMenu
{
    Q_OBJECT

public:
    PopupWindow (QWidget *parent = 0, const QString& txt = "");
    ~PopupWindow ();
};

#endif
