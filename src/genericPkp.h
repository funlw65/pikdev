/***************************************************************************
                          genericPkp.hh  -  description
                             -------------------
    begin                : Sun Feb 5 2006
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
/* This is the generic pic class, for ALL pics supported by pkp
   @author Gibaud Alain */

#ifndef GENERICPKP_HH
#define GENERICPKP_HH

#include "pic.hh"


class genericPkp : public pic
{
    bool readDevRev (string& dev, string& rev);
  
public:
    genericPkp ();
    virtual int verify_pic (ostream& out); // Verify the PIC's memory against memory buffer.
    virtual int program_pic (ostream& out); // Program  RAM buffer into the PIC
    virtual void read_pic (); // Read the PIC memory and fill the memory buffer.
    virtual int erase_pic (); // Erase all the informations programed into the device
    virtual string read_device_name ();
    virtual string read_device_revision ();
};

#endif
