/***************************************************************************
                     pk2cmdProgrammer.cpp  -  description
                             -------------------
    begin                : Thu Jul 16 2008
    copyright            : (C) 2008 by Gibaud Alain
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
#include "genericPk2cmd.h"
#include "pk2cmdProgrammer.h"

#include <QApplication>
#include <QFileDialog>


pk2cmdProgrammer::pk2cmdProgrammer ()
{
    thepic = new genericPk2cmd;
}

pk2cmdProgrammer::~pk2cmdProgrammer ()
{
    delete thepic;
    thepic = 0;
}

ConfigWidget *pk2cmdProgrammer::configWidget ()
{
    return 0; // no config needed
}

QString pk2cmdProgrammer::shortName ()
{
    return "pk2cmd";
}

QString pk2cmdProgrammer::longName ()
{
    return tr ( "pk2cmd (version >= 1.20) external programmer for Microchip picKit2" ) ;
}

QString pk2cmdProgrammer::exit ()
{
    return "";
}

QString pk2cmdProgrammer::enter ()
{
    QString pk2, ret;
    char str[300], version[100];
    Config::getToolPath ("pk2cmd", pk2);
    pk2 += " -?V 2>&1";
    FILE *f = popen (pk2.toLatin1 (), "r");

    if (f != 0)
      {
	while (fgets (str, 300, f) != 0)
	  {
	    if (sscanf (str, "Executable Version:%s", version) == 1)
	      {
		QString ver (version);
		if (ver < "1.20.00")
		  {
		    ret = "<b>Please upgrade pk2cmd</b><br><hr>"
		          "Current version: V" + ver + "<br>"
		          "Required: >= V1.20.0<br><br>" 
		          "see  <i>http://www.microchip.com/pickit2</i>";
		  }
	      }
	  }
	pclose (f);
      }
    return ret;
}

bool pk2cmdProgrammer::selectDevice (const QString& devname)
{
    return thepic->instanciate (devname.toLatin1 ());
}

QStringList pk2cmdProgrammer::devices ()
{
    // as we need ALL device names together, there is no reason
    // to use something else than popen
    QString pk2;
    Config::getToolPath ("pk2cmd", pk2);
    pk2 += " -?P 2>&1";

    FILE *f = popen (pk2.toLatin1 (), "r");
    char name[30], str[300];
    QStringList list;

    if (f != 0)
      {
	while (fgets (str, 300, f) != 0)
	  {
	    // lines containing devices name always begin by PIC...
	    if (sscanf (str, "PIC%s", name) == 1)
	      {
		QString devname (name);
		list += "p" + devname.toLower ();
	      }
	  }
	pclose (f);
      }
    else
      {
	clearConsole ();
	addTextConsole ("Cannot contact pk2cmd programmer");
      }
    return list;
}

bool pk2cmdProgrammer::connectionOK ()
{
    QString pk2;
    Config::getToolPath ("pk2cmd", pk2);
    pk2 += " -PF 2>&1";
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

QString pk2cmdProgrammer::extraFct ()
{
    QString opt;

    opt += tr ("{Turn target power on}");
    opt += tr ("{Turn target power off}");
    opt += tr ("{Turn target power on, release MCLR}");
    opt += tr ("{Turn target power off, release MCLR}");
    opt += tr ("{Firmware download to PicKit2}");

    return opt;
}

bool pk2cmdProgrammer::executeExtraFct (int function_number)
{
    bool r = true;

    QString pk2;
    Config::getToolPath ("pk2cmd", pk2);
    char buf[400], c;

    if (function_number == 0)
      pk2 += " -P -T 2>&1"; 
    else if (function_number == 1)
      pk2 += " -P  2>&1" ;
    else if (function_number == 2)
      pk2 += " -P -T -R 2>&1";
    else if (function_number == 3)
      pk2 += " -P -R 2>&1" ;
    else if (function_number == 4)
      {
	QString filename = QFileDialog::getOpenFileName (0,
							 tr ("Please select a Microchip firmware file"),
							 QString ("."),
							 tr ("Intel hex files (*.hex *.HEX);;All files (*)"));



	if (filename == "")
	  return false;

	pk2 += " -D"; // filename MUST be immediatly after option letter (/xx/yy/file.hex bug)
	pk2 += filename;
	pk2 += " 2>&1";
      }
    else
      return false;

    FILE *f = popen (pk2.toLatin1 (), "r");
    if (f == 0)
      return false;

    qApp->processEvents ();

    while (fgets (buf, 400, f) != 0)
      {
	sscanf (buf, "%[^\n]", buf); // remove trailing newline, if any
	if (sscanf (buf, "No PICkit 2 found%c", &c) == 1
	    || sscanf (buf, "Error opening hex file%c", &c) == 1)
	  {
	    r = false;
	    QApplication::beep ();
	    clearConsole ();
	    addTextConsole (QString (buf));
	  }
	else if (sscanf (buf, "OS Update Successful%c", &c) == 1
		 || sscanf (buf, "Downloading OS%c", &c) == 1
		 || sscanf (buf, "Verifying new OS%c", &c) == 1
		 || sscanf (buf, "Resetting PICkit 2%c", &c) == 1)
	  {
	    addTextConsole (QString (buf));
	    qApp->processEvents ();
	  }
      }

    if (pclose (f) != 0)
      return false;
    return r;
}
