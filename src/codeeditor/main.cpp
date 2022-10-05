#include <QtGui>

#include "Appli.h"


int main (int argv, char **args)
{
    QApplication app (argv, args);
    Appli a;
    a.show ();
    return app.exec ();
}
