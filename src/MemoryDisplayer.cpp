#include <iostream>
using namespace std;

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QPalette>
#include <QRegExp>
#include <QScrollBar>
#include <QToolTip>

#include "MemoryDisplayer.h"


MemoryDisplayer::MemoryDisplayer(QWidget *parent,
                                 FragBuffer& mem,
                                 unsigned int ind, unsigned int siz,
                                 unsigned int lin, unsigned int col,
                                 const QString& title,
                                 unsigned int mask,
                                 bool byteAddr,
                                 const char *sploc_descriptor)
        : QGroupBox (title,parent), memory (mem), begin_display (ind),
	  begin (ind), size (siz), lines (lin), cols (col), byte_addr (byteAddr)
{
    QHBoxLayout *Lthis = new QHBoxLayout (this);

    decodeSPDescriptor (sploc_descriptor); // update specialLocs map

    HexValueEditor *h;

    // address column
    QFrame *adr = new QFrame (this);
    Lthis->addWidget (adr);
    QGridLayout *Ladr = new QGridLayout (adr);
    Ladr->setMargin (3);

    adr->setToolTip (byteAddr ? tr ("Bytes Adresses") : tr ("Words Adresses"));
    adr->setFrameStyle (QFrame::Panel | QFrame::Sunken);
    adresses = new QList<QLabel *>;
    for (unsigned int k = 0; k < lines; ++k)
      {
        QLabel *l = new QLabel ("00000000", adr);
        Ladr->addWidget (l, k, 0);
        adresses->append (l);
      }
    adr->setMinimumWidth (fontMetrics ().size (Qt::TextSingleLine, "DDDDDDDD").width ());

    // memory words
    QFrame *grid = new QFrame (this);
    Lthis->addWidget (grid);
    QGridLayout *Lgrid = new QGridLayout (grid);

    Lgrid->setMargin (3);
    grid->setFrameStyle (QFrame::Panel | QFrame::Sunken);
    QPalette pal = grid->palette ();
    pal.setColor (QPalette::Window, QColor (255, 255, 255));
    grid->setPalette (pal);
    grid->setAutoFillBackground (true);

    int hh = fontMetrics ().size (Qt::TextSingleLine, "X").height () * 2 + 20;

    h = 0;
    HexValueEditor *h1;
    unsigned int icol = 0, iline = 0;
    for (unsigned int i = begin; i < (cols * lines) + begin; ++i)
      {
        h1 = new HexValueEditor (grid, 0, mask);
        Lgrid->addWidget (h1, iline, icol);
        if (++icol >= cols)
	  {
	    icol = 0;
	    ++iline;
	  }
        if (h != 0)
	  h->setNextEditor (h1);
        else
	  firstEditor = h1;
        h = h1;
      }
    setBase (begin);

    // scrollbar if there is more lines to display
    // than available lines
    int asw = 0;
    if ((size / cols) > lines)
      {
        QScrollBar *as = new QScrollBar (Qt::Vertical, this);
        Lthis->addWidget (as);

        int min = begin / cols;
        int max = ((size + begin) / cols) - lines;
        if (size % cols != 0)
	  ++max;
        if (max < 0)
	  max = 0;

        // max must be set before min
        as->setMaximum (max);
        as->setMinimum (min);
        connect (as, SIGNAL (valueChanged (int)), this, SLOT (set1stLine (int)));
        asw = as->width ();
      }

    setMinimumWidth (h->sizeHint ().width () * (cols + 1) + 10 + asw);
    setMaximumHeight (h->sizeHint ().height () * lines + hh);
    setMinimumHeight (h->sizeHint ().height () * lines + hh);
}

void MemoryDisplayer::setBase (unsigned int ind)
{
    if (ind < begin || ind >= size + begin)
      return;

    // for optimization purpose
    int addr_offset = ind - begin_display;
    
    begin_display = ind;
    HexValueEditor *h;
    unsigned int i;
    QColor normal (0xFFFFFF);

    // avoid to recompute the past-end iterator during loop
    map<int, pair<int, QString>>::iterator the_end = specialLocs.end ();
    // memory
    for (i = begin_display, h = firstEditor; h != 0; ++i, h = h->nextEditor ())
      {
        h->setNewAdress ((i >= size + begin) ? 0 : memory + i);
        if (specialLocs.find (i) != the_end)
	  {
	    // this is a special location
	    h->setColor (QColor (specialLocs[i].first));
	    h->setToolTip (specialLocs[i].second);
	  }
        else if (specialLocs.find (i - addr_offset) != the_end)
	  {
	    // this widget contained a special location. Turn it to normal.
	    h->setColor (normal);
	    h->setToolTip ("");
	  }
        /* else, the new location is not special,
           and the previous one was not too => nothing to do */
      }

    QList<QLabel *>::iterator p;
    for (p = adresses->begin (), i = begin_display; p != adresses->end (); i += cols, ++p)
      (*p)->setText (QString::asprintf ("%08X", byte_addr ? i * 2 : i));
}

void MemoryDisplayer::set1stLine (int line_number)
{
    unsigned int baseadr = (unsigned int) line_number * cols;
    setBase (baseadr);
}

void MemoryDisplayer::updateDisplay ()
{
    setBase (begin_display);
}

void MemoryDisplayer::decodeSPDescriptor (const char *descriptor)
{
    QRegExp r ("[ ]*([0-9a-fA-F]+|\\[[0-9a-fA-F]+-[0-9a-fA-F]+\\])(?:=#([0-9a-fA-F]+))?(?:\\{([^\\}]+)\\})?");
    QRegExp r1 ("\\[([0-9a-fA-F]+)-([0-9a-fA-F]+)\\]");

    int low, hi, color = 0xFFFF6B /* yellow */, offset = 0; 

    while (r.indexIn (descriptor + offset) != -1)
      {
        offset += r.matchedLength ();

        if (r1.indexIn (r.cap (1)) == -1)
	  {
            low = hi = r.cap (1).toInt (0, 16);
	  }
        else
	  {
            low = r1.cap (1).toInt (0, 16);
            hi = r1.cap (2).toInt (0, 16);
	  }

        if (r.cap (2) != "")
	  color = r.cap (2).toInt (0, 16);
        // else keep default or previous color

        // now, populate map
        if (hi < low)
	  {
	    int t = hi;
	    hi = low;
	    low = t;
	  }
        for (int i = low; i <= hi; ++i)
	  specialLocs[i] = make_pair (color, r.cap (3));
      }
}
