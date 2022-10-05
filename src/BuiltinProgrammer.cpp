/***************************************************************************
                      BuiltinProgrammer.cpp  -  description
                             -------------------
    begin                : Tue Jan 31 2006
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

#include <QMessageBox>

#include "BuiltinProgrammer.h"
#include "Config.h"
#include "PicFactory.hh"
#include "PrgConfigWidget.h"


BuiltinProgrammer::BuiltinProgrammer ()
{
}

BuiltinProgrammer::~BuiltinProgrammer ()
{
}

ConfigWidget *BuiltinProgrammer::configWidget ()
{
    ConfigWidget *c = new PrgConfigWidget (&hard);
    return c;
}

QString BuiltinProgrammer::shortName ()
{
    return "Builtin";
}

QString BuiltinProgrammer::longName ()
{
    return tr ("Builtin PiKdev programmer");
}

QString BuiltinProgrammer::exit ()
{
    if (thepic)
      thepic->setPort (0);
    hard.releasePort ();
    return "";
}

QString BuiltinProgrammer::enter ()
{
    QString r;
    // read current config
    QString type, port;
    int vpp, vdd, clock, dataout, datain, datarw, clkdelay;
    Config::getCurrentPortConfig (type, port, vpp, vdd, clock,
				  dataout, datain, datarw, clkdelay);

    // for safe conversion to const char*
    QByteArray bat = type.toLatin1 (), bap = port.toLatin1 ();

    // allocate port
    hard.setpins (bat, bap, vpp, vdd, clock, dataout, datain, datarw, clkdelay);

    if (! hard.isok ())
      {
        r = tr ("Cannot allocate %1 port %2.<br><br>"
                "Please&nbsp;use&nbsp;the configuration dialog<br>"
                "to setup your port configuration.").arg (type).arg (port);
      }
    else
      {
	// choose a default pic device: it will be overwritten later
	// just before calling programming widget
        if (selectDevice ("p16f628"))
          thepic->setPort (&hard);
        else
          r = tr ("Internal error in BuiltinProgrammer::enter()");
      }  
    return r;
}

bool BuiltinProgrammer::selectDevice (const QString& devname)
{
    QByteArray badev = devname.toLatin1 ();
    thepic = PicFactory::factory ()->getPic (badev);
    if (thepic)
      thepic->setPort (&hard);
    return thepic != 0;
}

QStringList BuiltinProgrammer::devices ()
{
    QString validprocs (PicFactory::factory ()->getPicNames ().c_str ());
    return validprocs.split (" ");
}

bool BuiltinProgrammer::connectionOK ()
{
    return hard.isok ();
}
