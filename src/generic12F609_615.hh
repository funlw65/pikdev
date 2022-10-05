/***************************************************************************
                     generic12F609_615.hh  -  description
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
/* p12f609 p12f615 p16f616 p16f610 p12hv609 p12hv615 p6hv610 p16hv616
   @author Gibaud Alain */

#ifndef generic12F609_615_H
#define generic12F609_615_H

#include "pic16F.hh"


class generic12F609_615 : public pic16F
{
public: 
    generic12F609_615 ();
    virtual int erase_pic ();
    virtual int program_pic (ostream&);
    virtual const char *names ();
};

#endif
