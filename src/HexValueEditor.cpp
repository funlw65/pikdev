#include <cctype>

#include <QApplication>
#include <QLabel>
#include <QPalette>

#include "HexValueEditor.h"


HexValueEditor::HexValueEditor (QWidget *parent, unsigned int *dataptr,
				unsigned int m, HexValueEditor *nex)
    : QLineEdit (parent), data (dataptr), mask (m), next (nex)
{
    connect (this, SIGNAL (textChanged (const QString&)),
	     this, SLOT (tryUpdate (const QString&)));
    connect (this, SIGNAL (lostFocus ()), this, SLOT (slotLostFocus ()));
    setNewAdress (dataptr);
    setFrame (false);
    setMaxLength (4);
}

void HexValueEditor::tryUpdate (const QString& txt)
{
    if (data == 0)
      return;
    QString s = txt;

    for (int i = 0; i < s.length (); ++i)
      {
        int c = s[i].unicode ();
        if (! isxdigit (c))
	  {
            s.truncate (i);
            setText (s);
            QApplication::beep ();
            return;
	  }
      }

    unsigned int v = s.toUInt (0, 16) & mask;

    *data = v;
    emit bufferModified ();
}

void HexValueEditor::slotLostFocus ()
{
    if (data == 0)
      return;
    *data = text ().toUInt (0, 16) & mask;
    setText (QString::asprintf ("%04X", *data));
}

QSize HexValueEditor::sizeHint () const
{
    return fontMetrics ().size (Qt::TextSingleLine, "DDDD");
}

QSize HexValueEditor::minimumSizeHint () const
{
    return fontMetrics ().size (Qt::TextSingleLine, "DDDD");
}

HexValueEditor *HexValueEditor::nextEditor ()
{
    return next;
}

void HexValueEditor::setNextEditor (HexValueEditor *n)
{
    next = n;
}

void HexValueEditor::setNewAdress (unsigned int *p)
{
    if ((data = p) == 0)
      {
        setText ("NA");
        setEnabled (false);
      }
    else
      {
        setText (QString::asprintf ("%04X", *data & mask));
        setEnabled (true);
      }
}

void HexValueEditor::setColor (const QColor& color)
{
    QPalette pal = palette ();
    pal.setColor (QPalette::Window, color);
    setPalette (pal);
    setAutoFillBackground (true);
}
