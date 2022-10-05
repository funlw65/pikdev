/***************************************************************************
                        generic16F87xA.hh  -  description
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

#ifndef GENERIC16F87XA_HH
#define GENERIC16F87XA_HH

#include "pic16F.hh"


class generic16F87xA : public pic16F
{
public: 
    generic16F87xA ();
    virtual int erase_pic ();
    virtual int program_pic (ostream&);
    virtual const char *names ();
};

#endif
