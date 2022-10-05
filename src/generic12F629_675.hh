/***************************************************************************
                     generic12F629_675.hh  -  description
                             -------------------
    begin                : Tue Jan 24 2006
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
/* 12F629/675 & 16F630/676
   @author Gibaud Alain */

#ifndef GENERIC12F629_675_HH
#define GENERIC12F629_675_HH

#include "pic16F.hh"


class generic12F629_675 : public pic16F
{
public:
    generic12F629_675 ();
    virtual int erase_pic ();
    virtual int program_pic (ostream&);
    virtual const char *names ();
    virtual const char *SPLocs ();
};

#endif
