/***************************************************************************
                       pk2Programmer.cpp  -  description
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

#include <QApplication>

#include "Config.h"
#include "ConfigWidget.h"
#include "genericPk2.h"
#include "pk2Programmer.h"


pk2Programmer::pk2Programmer ()
{
    thepic = new genericPk2;
}
pk2Programmer::~pk2Programmer ()
{
    delete thepic;
    thepic = 0;
}

ConfigWidget *pk2Programmer::configWidget ()
{
    return 0; // no config needed
}

QString pk2Programmer::shortName ()
{
    return "pk2";
}

QString pk2Programmer::longName ()
{
    return tr ("pk2 external programmer");
}

QString pk2Programmer::exit ()
{
    // thepic->setPort (0);
    return "";
}

QString pk2Programmer::enter ()
{
    // nothing to do
    // could probe pk2
    return "";
}

bool pk2Programmer::selectDevice (const QString& devname)
{
    return thepic->instanciate (devname.toLatin1 ());
}

QStringList pk2Programmer::devices ()
{
    // as we need ALL device names together, there is no reason
    // to use something else than popen
    QString pk2;
    Config::getToolPath ("pk2", pk2);
    pk2 += " --list 2>&1";
    FILE *f = popen (pk2.toLatin1 (), "r");
    char str[300], name[30];
    int k, j;
    QStringList list;

    if (f != 0)
      {
        while (fgets (str, 300, f) != 0)
	  {
            // lines containing devices name always begin by a digit
            if (isdigit (*str))
	      {
		// split
                for (k = 0; sscanf (str + k, "%s%n", name, &j) == 1; k += j)
		  {
                    QString devname (name);
                    list += "p" + devname.toLower ();
		  }
	      }
	  }
        pclose (f);
      }
    else
      {
        clearConsole ();
        addTextConsole ("Cannot contact pk2 programmer");
      }
    return list;
}

bool pk2Programmer::connectionOK ()
{
    QString pk2;
    Config::getToolPath ("pk2", pk2);
    pk2 += " --version 2>&1";
    FILE *f = popen (pk2.toLatin1 (), "r");
    char str[300];
    bool r = false;

    if (f != 0)
      {
        while (fgets (str, 300, f) != 0)
	  {
	    ;
	  }
        pclose (f);
        r = true;
      }
    return r;
}

QString pk2Programmer::extraFct ()
{
    QString opt;
    opt += tr ("{Turn target power on}");
    opt += tr ("{Turn target power off}");
    opt += tr ("{Power cycle the target}");
    return opt;
}

bool pk2Programmer::executeExtraFct (int function_number)
{
    QString pk2;
    Config::getToolPath ("pk2", pk2);
    char buf[400], dummy[100]; 
    bool r = true;

    if (function_number == 0)
      pk2 += " --on 2>&1";
    else if (function_number == 1)
      pk2 += " --off 2>&1";
    else if (function_number == 2)
      pk2 += " --reset 2>&1";
    else
      return false;

    FILE *f = popen (pk2.toLatin1 (), "r");
    if (f == 0)
      return false;

    while (fgets (buf, 400, f) != 0)
      {
	if (sscanf (buf, "Fatal error>%[^\n]", dummy) == 1)
	  {
	    r = false;
	    qApp->beep ();
	    clearConsole ();
	    addTextConsole (QString (dummy));
	  }
      }
    if (pclose (f) != 0)
      return false;
    return r;
}
