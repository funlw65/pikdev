/***************************************************************************
                        BinWordsEditor.h  -  description
                             -------------------
    begin                : Thu Jun 26 2003
    copyright            : (C) 2003 by Alain Gibaud
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
/* Multi Words binary editor for config registers
   @author Alain Gibaud */

#ifndef BINWORDSEDITOR_H
#define BINWORDSEDITOR_H

#include <QGroupBox>
#include <QStackedWidget>

#include "BinWordEditor.h"
#include "FragBuffer.hh"


class BinWordsEditor : public QGroupBox
{
    Q_OBJECT
    QStackedWidget *stack;

public: 
    BinWordsEditor (FragBuffer& me, unsigned int index1, unsigned int index2,
                    const QString& desc, const QString& title, bool byteAddr,
		    QWidget *parent);
    virtual ~BinWordsEditor ();

public slots:

signals:
    void wordsChanged ();
};

#endif
