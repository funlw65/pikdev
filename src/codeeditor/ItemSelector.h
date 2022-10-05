/***************************************************************************
    begin                : 2013
    copyright            : (C)  2013 by Alain Gibaud
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

#ifndef ITEMSELECTOR_H
#define ITEMSELECTOR_H

#include <QDialog>
#include <QListWidget>
#include <QString>
#include <QVector>
#include <QWidget>


class ItemSelector : public QDialog
{
    Q_OBJECT
    QListWidget *list;

public:
    enum {Cancel = 0, Selected = 1, All = 2};

    ItemSelector (const QString& title, QWidget *parent,
		  const QString& button1, const QString& button2,
		  const QString& button3);
    QVector<int> results ();
    void addItem (const QString& item, int itemId);
};

#endif // ITEMSELECTOR_H
