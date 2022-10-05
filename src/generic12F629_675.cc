/***************************************************************************
                     generic12F629_675.cc  -  description
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

#include "generic12F629_675.hh"


generic12F629_675::generic12F629_675 ()
{
}

const char *generic12F629_675::names ()
{
    return "p12f629 p12f675 p16f630 p16f676";
}

/* For this device the OSCCAL and BG bits must be preserved before
   erasing, and restored after erasing. */
int generic12F629_675::erase_pic ()
{
    unsigned int cfw, BGbits, CALword, dummy = 0x3FFF;

    progress_status = 2;
    
    // read OSCCAL value (location == 0x3FF)
    pulseEngine ("cdP,B,");
    for (int x = memaddr1; x < memaddr2; x++)
      pulseEngine("k6,"); // goto last word
    pulseEngine ("k4,R,", &CALword);
    //read BG bits
    pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,k4,R,", &cfw);
    BGbits = cfw | 0x1FF; // force 9 lower bits to '1'
    // TODO : use PIKDB data for this purpose
    --progress_status;
    // bulk erase program memory and data memory
    pulseEngine ("k0,S,k9w9000k11w9000", &dummy);
    // shutdown
    pulseEngine ("bpcd");

    // reenter programming mode
    pulseEngine ("cdP,B,");
    for (int x = memaddr1; x < memaddr2; x++)
      pulseEngine ("k6,");  // goto last word
    pulseEngine ("k2,S,k8w7000", &CALword);  // reprogram calibration word
    pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,k2,S,k8w7000", &BGbits);  // reprogram BG
    pulseEngine ("bpcd");
    --progress_status;

    return 0;
}

int generic12F629_675::program_pic (ostream& out)
{
    int stat, n;
    int x;
    unsigned int chip;

    erase_pic ();
    stat = 0;

    initProgress ();
    --progress_status;
    
    pulseEngine ("cdP,B,");

    // the last word (memaddr2 sddr) must not be programmed (osscal word)
    for (x = memaddr1; x < memaddr2; x++, --progress_status)
      {
        if (pulseEngine("k4,F,V", mem + x)) // do not overwrite correct data
	  {
            n = pulseEngine ("k2,S,k8w10000k4,F,V,", mem + x);
            if (n != 0)
	      {
                stat |= 1; // report failure
                pulseEngine ("?", &chip); // get chip value
                warning (out, "Prog memory", x, chip, mem[x]);
	      }
	  }
        pulseEngine("k6,");
      }
    pulseEngine ("k6,"); // skip the last word

    // Program the EEPROM data area
    for (x = eeaddr1; x <= eeaddr2; x++, --progress_status)
      {
        if (pulseEngine ("k4,F,V", mem + x)) // do not overwrite correct data
	  {
            n = pulseEngine ("k3,S,k8w10000k5,f,v,", mem + x);
            if (n != 0)
	      {
                stat |= 2; // report failure
                pulseEngine ("?", &chip);
                warning (out, "EEPROM data", x, chip, mem[x]);
	      }
	  }
        pulseEngine ("k6,");
      }

    // Program  ID configurations
    // enter configuration mode
    unsigned int dummy;
    pulseEngine ("k0,S,", &dummy);

    for (x = idaddr1; x <= idaddr2; x++, --progress_status)
      {
        n = pulseEngine ("k2,S,k8w10000k4,F,V,k6,", mem + x);
        if (n != 0)
	  {
            stat |= 4;   // report failure
            pulseEngine ("?", &chip);
            warning (out, "ID data", x, chip, mem[x]);
	  }
      }

    // program the config word : we must read it to preserve BG bits
    unsigned int cf;
    n = pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,k4,R,", &cf);
    cf &= 0x3000;
    if (cf != (mem[cfaddr1] & 0x3000))
      {
        warning (out, "BG bits have been preserved", cfaddr1);
        stat |= 32;
      }
    cf |= (mem[cfaddr1] & ~0x3000);
    pulseEngine ("k2,S,k8w10000,k4,F,V,", &cf);
    if (n != 0)
      {
        stat |= 8;
        pulseEngine ("?", &chip);
        warning (out, "Config fuses", cfaddr1, chip, cf);
      }
    --progress_status;
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdbp");

    return stat;
}

const char *generic12F629_675::SPLocs ()
{
    static char buf[100];
    sprintf (buf, "%X{OSCCAL calibration word}", memaddr2); 
    return buf;
}
