#include "XQPushButton.h"


XQPushButton::XQPushButton (const QString& text, QWidget *parent, int identifier)
    : QPushButton (text, parent), ident (identifier)
{
    connect (this, SIGNAL (clicked ()), this, SLOT (slotClicked ()));
}

XQPushButton::~XQPushButton ()
{
}

int XQPushButton::getID ()
{
    return ident;
}

void XQPushButton::slotClicked ()
{
    emit (clicked (ident));
}
