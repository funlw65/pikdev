/* A simple draggable label to move text
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef DRAGGABLELABEL_H
#define DRAGGABLELABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QWidget>

#include "CCodeGenerator.h"


class DraggableLabel : public QLabel
{
    Q_OBJECT

public:
    DraggableLabel (QWidget *parent, const QString& lab, CCodeGenerator *cg, int selector);
    ~DraggableLabel ();
    virtual void mousePressEvent (QMouseEvent *e);

private:
    CCodeGenerator *Cgenerator;
    int selector;
};

#endif
