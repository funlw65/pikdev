#ifndef APPLI_H
#define APPLI_H

#include <QMainWindow>
#include <QMenu>

#include "QBEditor.h"

class Appli : public QMainWindow
{
    Q_OBJECT
    QBEditor *edit;

public:
    Appli ();

public slots:
    void slotQuit ();
    void slotExperiment ();
};

#endif // APPLI_H
