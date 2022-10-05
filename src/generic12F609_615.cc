/***************************************************************************
                     generic12F609_615.cc  -  description
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

#include "generic12F609_615.hh"


// THIS CLASS IS NOT COMPLETED OR TESTED
generic12F609_615::generic12F609_615 ()
{
}

const char *generic12F609_615::names ()
{
    return "p12f609 p12f615 p16f616 p16f617 p16f610 p12hv609 p12hv615 p6hv610 p16hv616";
}

// Uses erasing mode which preserves config bits
int generic12F609_615::erase_pic ()
{
    unsigned int dummy = 0x3FFF;

    // this is for pikdev progress bar not mandatory, but useful. 
    // It means that this operation contains two phases.
    progress_status = 2;
    // classic power up sequence :
    // set clock and datalines low,
    // Power up the programmer, wait 1 us
    // set VPP hi, wait 1 us
    pulseEngine ("cdP,B,");
    // step finished
    --progress_status;
    // enter load configuration mode (cmd 0), wait 1us, 
    // send dummy argument, wait
    // send "Bulk erase program memory" (command 9) and wait 6ms
    pulseEngine ("k0,S,k9w6000", &dummy);
    // shutdown:
    // VCC low, VPP low
    // data and clock low
    pulseEngine ("bpcd");

    // step finished
    --progress_status;

    return 0;
}

int generic12F609_615::program_pic (__attribute__ ((unused)) ostream& out)
{
#if 0
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
	if (pulseEngine ("k4,F,V", mem + x)) // do not overwrite correct data
	  {
	    n = pulseEngine ("k2,S,k8w10000k4,F,V,", mem + x);
	    if (n != 0)
	      {
		stat |= 1;   // report failure
		pulseEngine ("?", &chip); // get chip value
		warning (out, "Prog memory", x, chip, mem[x]);
	      }
	  }
	pulseEngine ("k6,");
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
		stat |= 2;   // report failure
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
	    stat |= 4; // report failure
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
#endif
    return 0;
}
