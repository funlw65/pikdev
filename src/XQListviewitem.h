/* Special QListViewItem for Project Widget
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef XQLISTVIEWITEM_H
#define XQLISTVIEWITEM_H

#include <QPainter>
#include <QString>
#include <QStringList>
#include <QTreeWidgetItem>


class XQListViewItem : public QTreeWidgetItem
{
public:
    XQListViewItem (QTreeWidgetItem *parent, const QStringList& labels);
    virtual ~XQListViewItem ();
    virtual int compare (QTreeWidgetItem* i, int col, bool ascending) const;
};

#endif
