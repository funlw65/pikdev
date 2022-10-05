#include <cstdio>

#include "Utility.h"


Utility::Utility ()
{
}

void *Utility::strToP (const QString& s)
{
    void *p = 0;
    sscanf (s.toStdString ().c_str (), "%p", &p);
    return p;
}

QString Utility::pToStr (void *p)
{
    char t[100];
    *t = 0;
    sprintf (t, "%p", p);
    return t;
}
