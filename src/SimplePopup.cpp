#include "SimplePopup.h"


SimplePopup::SimplePopup (const QString& txt, QWidget *parent) : QMenu (parent)
{
    addAction (QIcon (":/src/icons/16/check.png"), txt)->setEnabled (false);
    addSeparator ();
}

int SimplePopup::exec (const QPoint& p)
{
    QAction *a = QMenu::exec (p);
    int r = -1;
    if (a)
      r = entries[a];
    return r;
}

QAction *SimplePopup::addItem (const QString& txt, int id)
{
    QAction *a = addAction (txt);
    entries[a] = id;
    return a;
}
