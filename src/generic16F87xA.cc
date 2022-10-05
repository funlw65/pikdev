/***************************************************************************
                       generic16F87xA.cc  -  description
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

#include "generic16F87xA.hh"


generic16F87xA::generic16F87xA ()
{
}

const char *generic16F87xA::names ()
{
    return "p16f873a p16f874a p16f876a p16f877a";
}

// Erase all the chip with the chip erase command
int generic16F87xA::erase_pic ()
{
    unsigned int dummy;

    // this device must be in configuration space (2000)
    // if we need to erase ALL the memory
    // I use the command 31 twice because, sometimes, the prog
    // memory is not totally erased. Not very elegant, but it works.
    pulseEngine ("cdpb,Pw100,Bw100,k0,S,k31w8000,k31w8000,c,d,b,p", &dummy);

    return 0;
}

/* -----------------------------------------------------------------------
   Program our RAM buffer into the PIC

   Returns:
   0 == OK
   1 == programm memory error
   2 == EEPROM data error
   4 == ID error
   8 == config fuses error

   the error codes are OR-ed
   ----------------------------------------------------------------------- */
int generic16F87xA::program_pic (ostream& out)
{
    int x;

    // seems to need to be erased before programming
    erase_pic ();

    initProgress ();
    // programming init sequence
    pulseEngine ("cdpb,P,B,w10", 0);

    // Program the PIC's main program code memory
    // 16F87XA prog memory must be programmed by 8 words bursts
    for (x = memaddr1; x <= memaddr2; --progress_status)
      {
        // load a 8 words block
        pulseEngine ("k2,S,k6,", mem + x); ++x;
        pulseEngine ("k2,S,k6,", mem + x); ++x;
        pulseEngine ("k2,S,k6,", mem + x); ++x;
        pulseEngine ("k2,S,k6,", mem + x); ++x;
        pulseEngine ("k2,S,k6,", mem + x); ++x;
        pulseEngine ("k2,S,k6,", mem + x); ++x;
        pulseEngine ("k2,S,k6,", mem + x); ++x;
        pulseEngine ("k2,S,", mem + x); ++x;
        pulseEngine ("k8w9000k6,"); // prog/erase , internally timed
        // pulseEngine ("k24w1000k23,k6,"); // prog only, externally timed
      }
    // program eeprom
    for (x = eeaddr1; x <= eeaddr2; x++, --progress_status)
      pulseEngine ("k3,S,k8,w8000,k6", mem + x);

    // program id data
    x = idaddr1;
    pulseEngine ("k0,S,k2,S,k6,", mem + x); ++x;
    pulseEngine ("k2,S,k6,", mem + x); ++x;
    pulseEngine ("k2,S,k6,", mem + x); ++x;
    pulseEngine ("k2,S,", mem + x); ++x;
    pulseEngine ("k8w9000,"); // prog/erase, internally timed
    --progress_status;

    // program config register
    pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,k2,S,k8,w9000,", mem + cfaddr1);
    --progress_status;

    // shutdown sequence
    pulseEngine ("cd,b,p", 0);

    return verify_pic (out);
}
