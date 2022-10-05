/***************************************************************************
                       generic16F6xxA.hh  -  description
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

#ifndef GENERIC16F6XXA_HH
#define GENERIC16F6XXA_HH

#include "pic16F.hh"


class generic16F6xxA : public pic16F
{
public: 
    generic16F6xxA ();
    virtual const char *names ();
    virtual int program_pic (ostream& o);
    virtual int erase_pic ();
};

#endif
