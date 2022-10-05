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

#ifndef P18HIGHLIGHTER_H
#define P18HIGHLIGHTER_H

#include <QtCore/qglobal.h>
QT_BEGIN_NAMESPACE
#include <QTextDocument>
QT_END_NAMESPACE

#include "HighlighterBase.h"


class P18Highlighter : public HighlighterBase
{
    Q_OBJECT

public:
    P18Highlighter (QTextDocument *parent = 0);

protected:
    void highlightBlock (const QString& text);

private:
    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
};

#endif // P18HIGHLIGHTER_H
