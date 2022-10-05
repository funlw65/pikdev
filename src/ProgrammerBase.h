/***************************************************************************
                        ProgrammerBase.h  -  description
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
/* Common framework for external (and internal) programmers
   @author Gibaud Alain */

#ifndef PROGRAMMERBASE_H
#define PROGRAMMERBASE_H

#include <iostream>
#include <sstream>
using namespace std;

#include <QString>
#include <QStringList>
#include <QObject>

#include "ConfigWidget.h"
#include "pic.hh"


class ProgrammerBase : public QObject, public progressObserver
{
    Q_OBJECT

protected:
    pic *thepic;

public:
    ProgrammerBase ();
    virtual ~ProgrammerBase ();
    // configurator widget for this programmer
    virtual ConfigWidget *configWidget () = 0;
    // name of this programmer
    virtual QString shortName () = 0;
    virtual QString longName () = 0;
    // List of supported devices
    virtual QStringList devices () = 0;
    // select a pic device
    virtual bool selectDevice (const QString& devname) = 0;
    // start using this programmer
    virtual QString enter () = 0;
    // stop using this programmer
    virtual QString exit () = 0;

    // current pic object
    pic *device ();

    // true if physical programmer can be reached
    virtual bool connectionOK () = 0;

    /* -----------------------------------------------------------------------
       Allows to inject into GUI access to functions
       that are not part of standard pikdev programming model 
       (but are available -and useful- for current programmer)
       Each of them will appear in the "extra" menu and take no parameter
       for now.

       Returns:
       A braces-enclosed list of item like this:
       {first item}{second item}

       Note:
       Default implementation returns an empty string.
       Function must be implemented in executeExtraFct (int function_number)
       ----------------------------------------------------------------------- */
    virtual QString extraFct ();

    /* -----------------------------------------------------------------------
       Execute extra function #function_number
    
       Returns:
       true if successful

       Note:
       Default implementation does nothing an returns true
       ----------------------------------------------------------------------- */
    virtual bool executeExtraFct (int function_number);
};

#endif
