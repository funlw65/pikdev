#include <iostream>
using namespace std;

#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>

#include "BinValueEditor.h"


BinValueEditor::BinValueEditor (unsigned int *wo, unsigned int bi,
				const QString& tx, QWidget *parent, bool editable)
    : QWidget (parent), label (tx), word (wo), bit (bi)
{
    QHBoxLayout *Lthis = new QHBoxLayout (this);
    Lthis->setSpacing (0);

    QLabel *l = new QLabel (QString::asprintf ("%2d", bi), this);
    Lthis->addWidget (l);
    int hh = l->fontMetrics ().horizontalAdvance ("DD");
    l->setMargin (0);
    l->setFixedWidth (hh);
    l->setFixedHeight (20);
    cb = new QCheckBox (tx, this);
    cb->setEnabled (editable);
    Lthis->addWidget (cb);

    connect (cb, SIGNAL (clicked ()), this, SLOT (updateWord ()));
    updateDisplay ();
}

void BinValueEditor::updateDisplay ()
{
    // force bits if they are named 0 or 1
    if (label == "0")
      *word &= ~(1 << bit);
    else if (label == "1")
      *word |= (1 << bit);

    cb->setChecked (*word & (1 << bit));
}

void BinValueEditor::updateWord ()
{
    if (cb->isChecked ())
      *word |= (1 << bit);
    else
      *word &= (~(1 << bit));
    emit bitChanged ();
}
