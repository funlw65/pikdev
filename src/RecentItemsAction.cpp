/***************************************************************************
                    RecentItemAction.cpp  -  description
                          -------------------
  Implements the "recent items" menus, using the Action paradigm

  Use addEntry () to add a new item. When an already existing entry is
  inserted, it is "renewed" (i.e.: it becomes the first entry in the list)

  Use addIconRule () to associate an Icon to a specific name pattern
  Typically, this functionnality allows to display an icon that is
  related to a type of file.

  The getNames () function return a list of current entries
  (the first items being the most recent))

    begin                : 2012
    copyright            : (C) 2012- by Alain Gibaud
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

#include <QMenu>

#include "RecentItemsAction.h"


// TODO : implement a limitation of menu size
RecentItemsAction::RecentItemsAction (const QString& txt, QObject *parent) :
    QAction (txt, parent)
{
    setMenu (new QMenu ());
    connect (menu (), SIGNAL (aboutToShow ()), this, SLOT (slotAboutToShow ()));
    connect (menu (), SIGNAL (triggered (QAction *)), this, SLOT (slotItemSelected (QAction *)));
    max_size = 20;
}

void RecentItemsAction::slotAboutToShow ()
{
    menu ()->clear ();
    for (int i = 0; i < names.count (); ++i)
      {
	QAction *a = menu ()->addAction (names[i]);

	for (int r = 0; r < regexps.count (); ++r)
	  {
	    if (regexps[r].exactMatch (names[i]))
	      {
		a->setIcon (icons[r]);
		break;
	      }
	  }
      }
}

void RecentItemsAction::addEntry (const QString& n)
{
    int kn = names.indexOf (n);

    if (kn > 0)
      names.removeAt(kn); // already exists and not first, remove it

    if (kn != 0)
      {
        // insert
        names.prepend (n);
        // !! max_size could be wrong !!
        while (names.count () > max_size && ! names.isEmpty ())
	  names.removeLast ();
      }
}

void RecentItemsAction::slotItemSelected (QAction *a)
{
    emit itemSelected (a->text ());
}

void RecentItemsAction::addIconRule (const QRegExp& reg, const QIcon& icon)
{
    regexps.append (reg);
    icons.append (icon);
}

const QStringList& RecentItemsAction::getNames () const
{
    return names;
}

void  RecentItemsAction::setMaxSize (int size)
{
    max_size = size;
}

int  RecentItemsAction::maxSize ()
{
    return max_size;
}

void RecentItemsAction::loadItems (const QString& collectionName, QSettings& settings)
{
    names.clear ();
    int size = settings.beginReadArray (collectionName);
    for (int i = 0; i < size; ++i)
      {
        settings.setArrayIndex (i);
        names += settings.value ("item").toString ();
      }
    settings.endArray ();
}

void RecentItemsAction::saveItems (const QString& collectionName, QSettings& settings)
{
    settings.beginWriteArray (collectionName);
    for (int i = 0; i < names.size (); ++i)
      {
        settings.setArrayIndex (i);
        settings.setValue ("item", names[i]);
      }
    settings.endArray ();
    settings.sync ();
}
