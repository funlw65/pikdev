/***************************************************************************
                          serport.hh  -  description
                             -------------------
    begin                : Mon Dec 22 2003
    copyright            : (C) 2003 by Alain Gibaud
    email                : alain.gibaud@univ-valenciennes.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* Serial port implementation,
   implements the port interface
   @author Alain Gibaud */

#ifndef SERPORT_HH
#define SERPORT_HH

#include <termios.h>

#include "port.hh"


class serport : public port
{
public: 
    serport ();
    ~serport ();
    /* Allocate and open the serial port
       The status of this operation can be tested with isok () */
    virtual void setport (const char *device);

    virtual void close ();
    bool isok ();
    virtual int readbit (int pin);

    // See bitoff (int)
    virtual void biton (int pin);

    // set the specified pin low
    virtual void bitoff (int pin);

    virtual const char *outputPinsList ();
    virtual const char *inputPinsList ();

    /* -----------------------------------------------------------------------
       Probes the port pname
 
       Returns:
       0 = the port is present on the system and is RW enabled
       -1 = port present but not RW enabled
       -2 = not present
       ----------------------------------------------------------------------- */
    static int probePort (const char *pname);

    // The previous state of the port. Used to restore before close.
    struct termios old_state;

private:
  // File descriptor of the port
  int fd;

  struct termios current_state;
};

#endif
