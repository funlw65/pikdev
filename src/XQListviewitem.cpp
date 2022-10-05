#include <iostream>
using namespace std;

#include <QFontMetrics>
#include <QPixmap>
#include <QToolTip>

#include "XQListviewitem.h"


XQListViewItem::XQListViewItem (QTreeWidgetItem *parent, const QStringList& labels) :
    QTreeWidgetItem (parent, labels)
{
    if (labels.count () >= 2)
      setToolTip (0, labels[1]);
}

XQListViewItem::~XQListViewItem ()
{
}

int XQListViewItem::compare (QTreeWidgetItem *i, __attribute__ ((unused)) int col,
			     __attribute__ ((unused)) bool ascending) const
{
    if (text (4) < i->text (4))
      return -1;
    else if (text (4) > i->text (4))
      return 1;
    // same type of symbol, sort by name
    if (text (0) < i->text (0))
      return -1;
    else if (text (0) > i->text (0))
      return 1;
    return 0;	
}
