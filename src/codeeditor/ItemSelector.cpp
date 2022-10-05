/* Button codes :
   0 = cancel
   1 = selected items
   2 = all items

   TODO: define constants */

#include <QGridLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QtGui>

#include "ItemSelector.h"


ItemSelector::ItemSelector (__attribute__ ((unused)) const QString& titletxt, QWidget *parent,
			    const QString& button1, const QString& button2,
			    const QString& button3) : QDialog (parent)
{
    setModal (true);

    QGridLayout *lay = new QGridLayout (this);
    QLabel *title = new QLabel (titletxt, this);
    lay->addWidget (title, 0, 0, 1, 3);

    QListWidget *list = new QListWidget (this);
    lay->addWidget (list, 1, 0, 1, 3);

    QPushButton *all = new QPushButton (button1, this);
    QPushButton *selected = new QPushButton (button2, this);
    QPushButton *cancel = new QPushButton (button3, this);

    lay->addWidget (all, 2, 0);
    lay->addWidget (selected, 2, 1);
    lay->addWidget (cancel, 2, 2);

    QSignalMapper *map = new QSignalMapper (this);
    connect (all, SIGNAL (clicked ()), map, SLOT (map ()));
    connect (selected, SIGNAL (clicked ()), map, SLOT (map ()));
    connect (cancel, SIGNAL (clicked ()), map, SLOT (map ()));
    map->setMapping (all, 2);
    map->setMapping (selected, 1);
    map->setMapping (cancel, 0);
    connect (map, SIGNAL (mapped (int)), this, SLOT (done (int)));
}

void ItemSelector::addItem (__attribute__ ((unused)) const QString& itemTxt, int itemId)
{
    QListWidgetItem *item = new QListWidgetItem (itemTxt, list);
    item->setData (Qt::UserRole, QVariant (itemId));
    item->setCheckState (Qt::Checked);
}

QVector<int> ItemSelector::results ()
{
    QVector<int> r;
    if (result () != ItemSelector::Cancel)
      {
        for (int i = 0; i < list->count (); ++i)
	  {
            QListWidgetItem *item = list->item (i);
            if (result () == ItemSelector::All || (item->checkState () == Qt::Checked))
	      r += item->data (Qt::UserRole).toInt ();
	  }
      }
    return r;
}
