/***************************************************************************
                     generic12_16F6xx.hh  -  description
                             -------------------
    begin                : Sun Jan 22 2006
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
/* @author Gibaud Alain */

#ifndef GENERIC12_16F6XX_HH
#define GENERIC12_16F6XX_HH

#include "pic16F.hh"


class generic12_16F6xx : public pic16F
{
public:
    generic12_16F6xx ();
    virtual void read_pic ();
    virtual int program_pic (ostream&);
    virtual int erase_pic ();
    virtual int verify_pic (ostream&);
    virtual const char *progOptions ();
    virtual const char *names ();
};

#endif
