#include <QDrag>
#include <QMimeData>

#include "DraggableLabel.h"


DraggableLabel::DraggableLabel (QWidget *parent, const QString& lab,
				CCodeGenerator *cg, int sel)
    : QLabel (lab, parent), Cgenerator (cg), selector (sel)
{
}

DraggableLabel::~DraggableLabel ()
{
}

void DraggableLabel::mousePressEvent (__attribute__ ((unused)) QMouseEvent *e) 
{
    QDrag *d = new QDrag (this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setText (Cgenerator->makeCode (selector));
    d->setMimeData (mimeData);
    d->exec ();
}
