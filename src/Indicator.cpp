#include <QColor>
#include <QPalette>
#include <QPixmap>

#include "Indicator.h"


Indicator::Indicator (QWidget *p) : QLabel (p)
{
    setOn (false);
    setFixedSize (sizeHint ());
    setAutoFillBackground (true);
}

bool Indicator::Indicator::isOn ()
{
    return on;
}

void Indicator::setOn (bool newon)
{
    on = newon;
#if 0
    static QPixmap idle (":/idle.png"), running (":/running.png");
    setPixmap (on ? running : idle);
#else
    QPalette pal = palette ();
    pal.setColor (QPalette::Window, on ? QColor (0xFF, 0, 0) : QColor (0xFF, 0xB5, 0xB5));
    setPalette (pal);
#endif
}

QSize Indicator::sizeHint ()
{
   return QSize (16, 16);
}
