/***************************************************************************
                          pkpProgrammer.cpp  -  description
                             -------------------
    begin                : Fri Feb 3 2006
    copyright            : (C) 2006 by Gibaud Alain
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

#include <cstdio>
#include <iostream>
using namespace std;

#include "Config.h"
#include "ConfigWidget.h"
#include "genericPkp.h"
#include "pkpProgrammer.h"


pkpProgrammer::pkpProgrammer ()
{
    thepic = new genericPkp;
}

pkpProgrammer::~pkpProgrammer ()
{
    delete thepic;
    thepic = 0;
}

ConfigWidget *pkpProgrammer::configWidget ()
{
    return 0; // no config needed
}

QString pkpProgrammer::shortName ()
{
    return "pkp";
}

QString pkpProgrammer::longName ()
{
    return tr ("pkp external programmer");
}

QString pkpProgrammer::exit ()
{
    //thepic->setPort (0);
    return "";
}

QString pkpProgrammer::enter ()
{
    // nothing to do
    // could probe pkp
    return "";
}

bool pkpProgrammer::selectDevice (const QString& devname)
{
    return thepic->instanciate (devname.toLatin1 ());
}

QStringList pkpProgrammer::devices ()
{
    // as we need ALL device names together, there is no reason
    // to use something else than popen
    QString pkp;
    Config::getToolPath ("pkp", pkp);
    pkp += " --list --mfmode";

    FILE *f = popen (pkp.toLatin1 (), "r");
    char name[60];
    QStringList list;

    if (f != 0)
      {
        while (fscanf (f, "%59s", name) != EOF)
	  list += name;
        pclose (f);
      }
    return list;
}

bool pkpProgrammer::connectionOK ()
{
    return true;
}
