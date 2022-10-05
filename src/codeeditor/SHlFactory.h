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

#ifndef SHLFACTORY_H
#define SHLFACTORY_H

#include <QList>
#include <QSettings>

#include "HighlighterBase.h"


class SHlFactory
{
private:
    SHlFactory ();
    static SHlFactory *myself;
    QList<HighlighterBase *> instances;

public:
    static SHlFactory *factory ();
    HighlighterBase *getSHl (const QString& filename);
    QStringList getSHlNames ();
    QList<HighlighterBase *> HLs ();
    void saveSettings (QSettings& settings);
    void restoreSettings (QSettings& settings);
};

#endif // SHLFACTORY_H
