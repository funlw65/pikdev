/***************************************************************************
                          pk2Programmer.h  -  description
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
/* Prototype class for testing external programmers support
   @author Gibaud Alain */

#ifndef PK2PROGRAMMER_H
#define PK2PROGRAMMER_H

#include "ConsoleClient.h"
#include "ProgrammerBase.h"


class pk2Programmer : public ProgrammerBase, public ConsoleClient
{
public:
    pk2Programmer ();
    ~pk2Programmer ();
    // configurator widget for this programmer
    virtual ConfigWidget *configWidget ();
    // name of this programmer
    virtual QString longName ();
    virtual QString shortName ();
    // List of supported devices
    virtual QStringList devices ();
    // Select a device. Return true if ok
    virtual bool selectDevice (const QString& devname);
    // start using this programmer
    virtual QString enter ();
    // stop using this programmer
    virtual QString exit ();
    // true if connexion media available
    virtual bool connectionOK ();
    virtual QString extraFct ();
    virtual bool executeExtraFct (int function_number);
};

#endif
