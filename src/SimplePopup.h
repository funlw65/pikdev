#ifndef SIMPLEPOPUP_H
#define SIMPLEPOPUP_H

#include <QAction>
#include <QMap>
#include <QMenu>
#include <QPoint>
#include <QString>
#include <QWidget>


class SimplePopup : public QMenu
{
    QMap<QAction *, int> entries;

public:
    SimplePopup (const QString& txt, QWidget *parent);
    int exec (const QPoint& p);
    QAction *addItem (const QString& txt, int id);
};

#endif // SIMPLEPOPUP_H
