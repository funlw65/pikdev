#include <cstdio>
#include <iostream>
using namespace std;

#include <QByteArray>
#include <QLayout>
#include <QToolTip>
#include <QVBoxLayout>

#include "BinValueEditor.h"
#include "BinWordEditor.h"
#include "MemoryDisplayer.h"


BinWordEditor::BinWordEditor (FragBuffer& me, unsigned int index,
			      const QString& desc,
                              __attribute__ ((unused)) bool byteAddr,
			      QWidget *parent)
    : QWidget (parent), word (me + index)
{
    QByteArray ba = desc.toLatin1 ();
    const char *p = ba;
    QString bitname, doc;
    int b1, b2;
    BinValueEditor *bve;

    QVBoxLayout *Lthis = new QVBoxLayout (this);
    Lthis->setSpacing (3);

    val = new QLabel ("[ ]", this);
    val->setAlignment (Qt::AlignCenter);
    Lthis->addWidget (val);

    while (parse (p, bitname, doc, b1, b2))
      {
        // the normal usage is to specify b1 >= b2
        // but this code can cope with b1 < b2
        if (b1 < b2)
	  {
            int k = b1;
	    b1 = b2;
	    b2 = k;
	  }
     
        QByteArray bn = bitname.toLatin1 ();

        for (int b = b1; b >= b2; --b)
	  {
            const QString bitname1 = QString::asprintf (bn, b - b2);
            bve = new BinValueEditor (word, b, bitname1, this,
                                      bitname != "0" && bitname != "1" && bitname != "X");
            Lthis->addWidget (bve);
            // special case : unimplemented bits cannot be edited
            if (doc != "")
	      bve->setToolTip (doc);
            connect(bve, SIGNAL (bitChanged ()), this, SLOT (updateHexDisplay ()));
            connect(this, SIGNAL (wordChanged ()), bve, SLOT (updateDisplay ()));
	  }
      }

    // some words only use 8 bits => recover the free space
    // to prevent the present bits from spreading out
    Lthis->addStretch (1);
}

bool BinWordEditor::parse (const char*& p, QString& name, QString& doc, int& b1, int& b2)
{
    char bitname[40];
    int a, b, n, i;

    name = "";
    while (*p == ' ')
      ++p;
    // XX[a-b] means XX0 ... XXa-b+1
    i = sscanf (p, "%[_$|A-Za-z0-9/-][%d-%d]%n", bitname, &a, &b, &n);
    if (i == 3)
      {
        name = bitname;
	b1 = a;
	b2 = b;
	p += n;
        name += "-%d";
        parseDoc (p, doc);
        return true;
      }
    // XX(a-b) means that all the bits from b to a are identical
    // and are (identically) named XX
    i = sscanf (p, "%[_$|A-Za-z0-9/-](%d-%d)%n", bitname, &a, &b, &n);
    if (i == 3)
      {
        name = bitname;
	b1 = a;
	b2 = b;
	p += n;
        parseDoc (p, doc);
        return true;
      }
    i = sscanf (p, "%[_$|A-Za-z0-9/-][%d]%n", bitname, &a, &n);
    if (i == 2)
      {
        name = bitname;
	b1 = a;
	b2 = a;
	p += n;
        parseDoc (p, doc);
        return true;
      }
    // XX[a] and XX(a) have the same meaning
    i = sscanf (p, "%[_$|A-Za-z0-9/-](%d)%n", bitname, &a, &n);
    if (i == 2)
      {
        name = bitname;
	b1 = a;
	b2 = a;
	p += n;
        parseDoc (p, doc);
        return true;
      }

    return false;
}

void BinWordEditor::updateHexDisplay ()
{
    val->setText (QString::asprintf ("<font color=#FF0000>[ %04X ]</font>", *word & 0xFFFF));
}

void BinWordEditor::updateDisplay ()
{
    updateHexDisplay ();
    emit wordChanged ();
}

unsigned int BinWordEditor::getValue ()
{
    return *word;
}

void BinWordEditor::parseDoc (const char*& p, QString& doc)
{
    doc = "";
    while (*p == ' ')
      ++p;
    if (*p == '{')
      {
        for (++p; *p && *p != '}'; ++p)
	  doc += *p;
        if (*p)
	  ++p;
      }
}
