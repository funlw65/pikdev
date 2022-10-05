/***************************************************************************
                      generic16F818_819.cc  -  description
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

#include "generic16F818_819.hh"


generic16F818_819::generic16F818_819 ()
{
}

int generic16F818_819::erase_pic ()
{
    unsigned int word = 0x3FFF;

    progress_status = 2;
    pulseEngine ("cdP,B,k0,S,k31,w4000,cdbp", &word); // tprog3 = 2ms minimum
    --progress_status;
    pulseEngine ("cdP,B,k0,S,k31,w4000,cdbp", &word); // tprog3 = 2ms minimum
    --progress_status;
    return 0;
}

int generic16F818_819::program_pic (ostream& o)
{
    int x;
    unsigned int dummy = 0x3FFF;

    // I use the "prog only" algorithm (DS39603C p10) so I must erase the chip
    erase_pic ();

    initProgress ();
    
    pulseEngine ("cdP,B");

    // Program the PIC's main program code memory
    for (x = memaddr1; x <= memaddr2; progress_status -= 4)
      {
        pulseEngine ("k2,S,k6,", &mem[x++]);
        pulseEngine ("k2,S,k6,", &mem[x++]);
        pulseEngine ("k2,S,k6,", &mem[x++]);
        pulseEngine ("k2,S,", &mem[x++]);
        pulseEngine ("k24w4000k23,k6,"); // tprog1 = 2ms minimum
      }
    // Program the EEPROM data area
    for (x = eeaddr1; x <= eeaddr2; ++x, --progress_status)
      pulseEngine ("k3,S,k24w4000k23,k6,", mem + x);

    // Program the ID configurations
    // enter configuration mode
    pulseEngine ("k0,S,", &dummy);

    for (x = idaddr1; x <= idaddr2; progress_status -= 4)
      {
        pulseEngine ("k2,S,k6,", &mem[x++]);
        pulseEngine ("k2,S,k6,", &mem[x++]);
        pulseEngine ("k2,S,k6,", &mem[x++]);
        pulseEngine ("k2,S,", &mem[x++]);
        pulseEngine ("k24w4000k23,k6,"); // tprog1 = 2ms minimum
      }

    // program the fuses
    pulseEngine ("k6,k6,k6,");
    for (x = cfaddr1; x <= cfaddr2; ++x, --progress_status)
      pulseEngine ("k2,S,k24w4000k23w100k6w100", &mem[x]);

    pulseEngine ("w1000cdbp");

    return verify_pic (o);
}

const char *generic16F818_819::names ()
{
    return "p16f818 p16f819 p16f87 p16f88";
}
