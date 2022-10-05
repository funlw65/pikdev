/***************************************************************************
                       generic16F7x.cc  -  description
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
/*

Copyright 2004 Keith Baker (syzygy@pubnix.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "generic16F7x.hh"


generic16F7x::generic16F7x ()
{
}

int generic16F7x::program_pic (ostream& out)
{
    int stat, n;
    int x;
    unsigned int word;
    stat = 0;
    unsigned int chip;

    // Added this because this chip can't selectively erase blcoks...
    // we clean up the chip first
    erase_pic ();

    initProgress ();
    
    // programming init sequence
    pulseEngine ("cdP,B,", 0);

    // Program the PIC's main program code memory
    for (x = memaddr1; x <= memaddr2; x++, --progress_status)
      {
        // Read Memory for two purposes,
        // 1) check to see if it needs to be altered
        // 2) If it does need to be altered, it better be 0x3FFF
        // otherwise device wasn't erased
        pulseEngine ("k4,R,", &word);
        if ((word != mem[x]) && (mem[x] != -1U) && (word != 0x3FFF))
	  warning (out, "Prog Device not Blank", x, chip, mem[x]);
        else if ((word != mem[x]) && (mem[x] != -1U))
	  {
            n = pulseEngine ("k2,S,k8w2000k14,k4,F,V", mem + x);
            if (n != 0)
	      {
                stat |= 1; // report failure
                pulseEngine ("?", &chip);
                warning (out, "Prog memory", x, chip, mem[x]);
	      }
	  }
        pulseEngine ("k6,"); // inc addr
      }

    // Program the ID configurations
    // enter configuration mode
    unsigned int dummy = 0x3FFF;
    pulseEngine ("k0,S,w2000", &dummy);
    for (x = idaddr1; x <= idaddr2; x++, --progress_status)
      {
        n = pulseEngine ("k2,S,k8w2000k14,k4,F,V,", mem + x);
        if (n != 0)
	  {
            stat |= 4; // report failure
            pulseEngine ("?", &chip);
            warning (out, "ID data", x, chip, mem[x]);
	  }
        pulseEngine ("k6,");
      }

    // program the fuses
    n = pulseEngine ("k6,k6,k6,k2,S,k8w2000k14,k4,F,V,", mem + cfaddr1);
    if (n != 0)
      {
        stat |= 8;
        pulseEngine ("?", &chip);
        warning (out, "Config fuses", cfaddr1, chip, mem[x]);
      }
    pulseEngine ("k6,"); // why ???
    --progress_status;

    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdbp");

    return stat;
}

int generic16F7x::erase_pic ()
{
    // Pic terminates erase itself...  just wait a while.
    pulseEngine ("cdP,B,k9w400000;cdbp");
    return 0;
}

const char *generic16F7x::names ()
{
    return "p16f73 p16f74 p16f76 p16f77";
}
