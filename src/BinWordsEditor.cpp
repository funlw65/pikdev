/***************************************************************************
                      BinWordsEditor.cpp  -  description
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
#include <iostream>
using namespace std;

#include <QComboBox>
#include <QFont>
#include <QFontMetrics>
#include <QLayout>
#include <QToolTip>
#include <QVBoxLayout>

#include "BinWordEditor.h"
#include "BinWordsEditor.h"


BinWordsEditor::BinWordsEditor (FragBuffer& me, unsigned int index1, unsigned int index2,
				const QString& desc, const QString& title, bool byteAddr,
				QWidget *parent)
    : QGroupBox (title, parent)
{
    QVBoxLayout *Lthis = new QVBoxLayout (this);
    QComboBox *chooser = new QComboBox (this);
    Lthis->addWidget (chooser);

    stack = new QStackedWidget (this);
    Lthis->addWidget (stack);
    BinWordEditor *we = 0;

    connect (chooser, SIGNAL (activated (int)), stack, SLOT (setCurrentIndex (int)));
    chooser->setToolTip (byteAddr ? tr ("Bytes addresses") : tr ("Words addresses"));
    
    for (unsigned int i = index1, k = 0; i <= index2; ++i, ++k)
      {
        const QString lab = QString::asprintf ("%6X", byteAddr ? 2 * i : i);
        chooser->addItem (lab);
        QString s = desc.section (";", k, k);
        if (s.isNull ())
	  s = "";

        we = new BinWordEditor (me, i, s, byteAddr, this);
        stack->addWidget (we);
        connect (this, SIGNAL (wordsChanged ()), we, SLOT (updateDisplay ()));
    }
    QFont f = font ();
    QFontMetrics fm (f);
    int h = fm.height ();
    setFixedHeight (32 * h);

    stack->setCurrentIndex (0);
    emit wordsChanged ();    
}

BinWordsEditor::~BinWordsEditor ()
{
}
