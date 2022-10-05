/***************************************************************************
                          16F.hh  -  description
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
/* Base abstract class for all 16F devices, with standard programming algorithms.

  As, there is no really standard algo for this family,
  subclasses will redefine most of them.

  @author Gibaud Alain */

#ifndef PIC16F_HH
#define PIC16F_HH

#include "pic.hh"


class pic16F : public pic
{
public: 
    pic16F ();
    virtual int verify_pic (ostream&);
    virtual int program_pic (ostream&);
    virtual void read_pic ();
    virtual int erase_pic ();
    virtual unsigned int get_deviceID ();
};

#endif
