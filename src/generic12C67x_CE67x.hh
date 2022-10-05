/***************************************************************************
                    generic12C67x_CE67x.hh  -  description
                             -------------------
    begin                : Tue Jan 17 2006
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
/* Covers 12C671/672/CE673/674
   DS40175
   @author Gibaud Alain */

#ifndef GENERIC12C67X_CE67X_HH
#define GENERIC12C67X_CE67X_HH

#include "generic12C5x8_5x9.hh"


class generic12C67x_CE67x : public generic12C5x8_5x9
{
public: 
    generic12C67x_CE67x ();
    virtual int program_pic (ostream&);
    virtual void read_pic ();
    virtual const char *SPLocs ();
    virtual const char *progOptions ();
    virtual const char *names ();
};

#endif
