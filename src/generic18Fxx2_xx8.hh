/***************************************************************************
                       generic18Fxx2_xx8.hh  -  description
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
/* Devices covered by DS39576B
   @author Gibaud Alain */

#ifndef GENERIC18FXX2_XX8_HH
#define GENERIC18FXX2_XX8_HH

#include <iostream>
using namespace std;

#include "pic18F.hh"


class generic18Fxx2_xx8 : public pic18F
{
public:
    generic18Fxx2_xx8 ();
    virtual void read_pic ();
    virtual int program_pic (ostream& out);
    virtual int verify_pic (ostream& o);
    virtual int erase_pic ();
    virtual const char *names ();
    virtual unsigned int get_deviceID ();
};

#endif
