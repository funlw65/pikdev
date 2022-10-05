/***************************************************************************
                      generic16F627_628.cc  -  description
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

#include "generic16F627_628.hh"


generic16F627_628::generic16F627_628 ()
{
}

const char *generic16F627_628::names ()
{
    return "p16f627 p16f628";
}

/* 16F628 seems to have problems with the standard 16F84 bulk
   erase when disabling code protection : the data memory is not
   set to 0xFF.
   This code adds a erase/programming pass on the data memory */
int generic16F627_628::erase_pic ()
{
    unsigned int allff = 0xFFFF;
    int stat = 0, n, x;

    // executes standard 16F84 erasing stuff
    stat |= pic16F::erase_pic ();
    // re-enter programming init sequence
    pulseEngine ("cdPw1Bw1", 0);

    // Program the EEPROM data area
    for (x = eeaddr1; x <= eeaddr2; x++)
      {
	n = pulseEngine ("k3w1Sw1k8w10000k5fv", &allff);
	if (n != 0)
	  stat |= 2; // report failure
	pulseEngine ("k6w1", 0); // inc addr
      }
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("w1bw1pw1", 0);
  
    return stat;
}
