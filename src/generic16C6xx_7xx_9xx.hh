/***************************************************************************
                    generic16C6xx_7xx_9xx.hh  -  description
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
/* @author Gibaud Alain */

#ifndef GENERIC16C6XX_7XX_9XX_HH
#define GENERIC16C6XX_7XX_9XX_HH

#include "pic.hh"


class generic16C6xx_7xx_9xx : public pic
{
public: 
    generic16C6xx_7xx_9xx ();
    virtual void read_pic ();
    virtual int erase_pic ();
    virtual int verify_pic (ostream&);
    virtual int program_pic (ostream& out);
    virtual const char *names ();
};

#endif
