/***************************************************************************
    begin                : 2013
    copyright            : 2013 by Alain Gibaud
    email                : alain.gibaud@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QSplashScreen>

#include "../config.h"
#include "PikMain.h"


int main (int argc, char *argv[])
{
    QApplication a (argc, argv);
    QCoreApplication::setOrganizationName ("GoldenBugSoftware");
    QCoreApplication::setOrganizationDomain ("alaingibaud.com");
    QCoreApplication::setApplicationName ("pikdev");
    PikMain *pikmain = new PikMain;
    pikmain->show ();    
    return a.exec ();
}
