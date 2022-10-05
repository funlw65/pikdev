/***************************************************************************
                     generic10f2xx_12f50x.hh  -  description
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
/* @author Gibaud Alain */

#ifndef GENERIC10F2XX_12F50X_HH
#define GENERIC10F2XX_12F50X_HH

#include "pic.hh"


class generic10F20x_12F50x : public pic
{
protected:
    int osccal_location; // osc calibration instruction, located at reset vector

public:
    generic10F20x_12F50x ();
    virtual int program_pic (ostream&);
    virtual int verify_pic (ostream&);
    virtual void read_pic ();
    virtual int erase_pic ();
    virtual const char *names ();
    virtual const char *SPLocs ();
    virtual const char *progOptions ();
    virtual void postInstanciate (const char *devname);
};

#endif
