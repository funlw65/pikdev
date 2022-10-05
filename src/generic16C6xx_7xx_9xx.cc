/***************************************************************************
                   generic16C6xx_7xx_9xx.cc  -  description
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

#include "generic16C6xx_7xx_9xx.hh"


generic16C6xx_7xx_9xx::generic16C6xx_7xx_9xx ()
{
}

int generic16C6xx_7xx_9xx::erase_pic ()
{
    return 16;  // 16 == NA
}

const char *generic16C6xx_7xx_9xx::names ()
{
    return "p16cr62 p16c62a p16c62b p16cr63 p16c63a p16cr64 p16c64a "
           "p16cr65 p16c65a p16c65b p16c66 p16c67 p16c71 p16c72 "
           "p16c72a p16cr73 p16c73a p16c73b p16cr74 p16c74a p16c74b "
           "p16cr76 p16cr77 p16c620 p16c620a p16cr620a p16c621 p16c621a p16c622 p16c622a "
           "p16ce623 p16ce624 p16ce625 p16c710 p16c711 p16c712 p16c716 p16c745 p16c765 "
           "p16c773 p16c774 p16c923 p16c924 p16c925 p16c926";
}

void generic16C6xx_7xx_9xx::read_pic ()
{
    int x;
    unsigned int dummy = 0x3FFF;

    fill_mem (); // fill progr buffer with INVALID pattern

    initProgress ();
    // enter programming  mode
    pulseEngine ("cdP,B,");
    // Read the PIC's  program memory
    for (x = memaddr1; x <= memaddr2; x++, --progress_status)
      pulseEngine ("k4,R,k6,", mem + x);

    // enter conf memory
    pulseEngine ("k0,S,", &dummy);
    for (x = idaddr1; x <= idaddr2; x++, --progress_status)
      pulseEngine ("k4,R,k6,", mem + x);

    // read Config Fuses
    pulseEngine ("k6,k6,k6,k4,R,", mem + cfaddr1);
    --progress_status;
    // power off sequence
    pulseEngine ("cdpb");
}

int generic16C6xx_7xx_9xx::program_pic (ostream& out)
{
    int stat, fail;
    int x;
    unsigned int dummy = 0x3FFF;

    stat = 0;

    // programming init sequence
    pulseEngine ("cdP,Bw100");

    initProgress ();

    for (x = memaddr1; x <= memaddr2; ++x, --progress_status)
      {
        // speed up : enter prog process only if word is not already correct
        if (mem[x] != INVALID && pulseEngine ("k4,F,V", mem + x) != 0)
	  {
            for (fail = 1; fail <= 25; ++fail)
	      if (pulseEngine ("k2,S,k8w100k14w5000k4,F,V", mem + x) == 0)
		break;
            if (fail > 25)
	      {
                pulseEngine ("k4,R,", &dummy);
                stat |= 1;
                warning (out, "Prog memory", x, dummy, mem[x]);
	      }
            else // 3 x fail additionnal passes
	      {
                for (fail *= 3; fail > 0; --fail)
		  pulseEngine ("k2,S,k8,w100,k14,w5000", mem + x);
	      }
	  }
        pulseEngine ("k6");
      }

    // enter configuration memory
    pulseEngine ("k0,S,", &dummy);

    for (x = idaddr1; x <= idaddr2; ++x, --progress_status)
      {
        for (fail = 1; fail <= 25; ++fail)
	  if (pulseEngine ("k2,S,k8w150k14w5000k4,F,V", mem + x) == 0)
	    break;
        if (fail > 25)
	  {
            pulseEngine ("k4,R,", &dummy);
            stat |= 4;
            warning (out, "ID memory", x, dummy, mem[x]);
	  }
        else // 3 x fail additionnal passes
	  {
	    for (fail *= 3; fail > 0; --fail)
	      pulseEngine ("k2,S,k8,w150,k14,w5000", mem + x);
	  }
        pulseEngine ("k6");
      }

    pulseEngine ("k6,k6,k6,");

    for (fail = 1; fail <= 100; ++fail)
      {
	// 100 cycles needed
        pulseEngine ("k2,S,k8w100k14w5000", mem + cfaddr1);
      }
    pulseEngine ("k4,R,", &dummy);
    if (dummy != mem[cfaddr1])
      {
        stat |= 8;
        warning (out, "Config fuses", cfaddr1, dummy, mem[cfaddr1]);
      }
    --progress_status;
    
    pulseEngine ("cdbp");

    return stat;
}

int generic16C6xx_7xx_9xx::verify_pic (ostream& out)
{
    FragBuffer ram;
    ram.swap (mem); // leave mem adresses unchanged

    int status = 0, i;

    read_pic ();

    // compare program memory
    for (i = memaddr1; i <= memaddr2; ++i)
      {
        if (mem[i] != ram[i] && ram[i] != INVALID)
	  {
            status |= 1;
            warning (out, "Prog memory", i, mem[i], ram[i]);
	  }
      }

    // compare ID
    for (i = idaddr1; i <= idaddr2; ++i)
      {
        if (mem[i] != ram[i] && ram[i] != INVALID)
	  {
            status |= 4;
            warning (out, "ID data", i, mem[i], ram[i]);
	  }
      }

    // compare config fuses
    if (mem[cfaddr1] != ram[cfaddr1] && ram[cfaddr1] != INVALID)
      {
        status |= 8;
        warning (out, "Config fuses", cfaddr1, mem[cfaddr1], ram[cfaddr1]);
      }

    // restore ram buffer
    mem.swap (ram);  // restore old buffer

    return status;
}
