/***************************************************************************
                     generic12C5x8_5x9.hh  -  description
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
/* Covers 12C508/509/508A/509A/CE518/CE519
   DS 30557D
   @author Gibaud Alain */

#ifndef GENERIC12C5X8_5X9_HH
#define GENERIC12C5X8_5X9_HH

#include "pic.hh"


class generic12C5x8_5x9 : public pic
{
public: 
    generic12C5x8_5x9 ();
    virtual int verify_pic (ostream&);
    virtual int program_pic (ostream&);
    virtual void read_pic ();
    virtual int erase_pic ();
    virtual const char *names ();
    virtual const char *SPLocs ();
    virtual const char *progOptions ();
};

#endif
