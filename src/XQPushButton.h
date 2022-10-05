/* A QPushButton with an int identifier
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef XQPUSHBUTTON_H
#define XQPUSHBUTTON_H

#include <QPushButton>
#include <QString>
#include <QWidget>


class XQPushButton : public QPushButton
{
    Q_OBJECT
    int ident;

public:
    XQPushButton (const QString& text, QWidget *parent = 0, int identifier = 0);
    virtual ~XQPushButton ();
    int getID ();

signals:
    void clicked (int);

private slots:
    void slotClicked ();
};

#endif
