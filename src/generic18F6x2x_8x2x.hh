/***************************************************************************
                      generic18F6x2x_8x2x.hh  -  description
                             -------------------
    begin                : Sun Jan 15 2006
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
/* All chips covered by DS39622 programming specifications:
   18F2xx0, 18F2x21, 18F2xx5, 18F4xx0, 18F4x21, 18F4xx5.
   @author Gibaud Alain */

#ifndef generic18F6x2x_8x2x_HH
#define generic18F6x2x_8x2x_HH

#include "pic18F.hh"


class generic18F6x2x_8x2x : public pic18F
{
protected:
    int vr_mode; // read verification mode, 0 by default
public:
    generic18F6x2x_8x2x ();
    virtual void read_pic ();
    virtual int program_pic (ostream& out);
    virtual int verify_pic (ostream& o);
    virtual int erase_pic ();
    virtual const char *names ();
    virtual unsigned int get_deviceID ();
};

#endif
