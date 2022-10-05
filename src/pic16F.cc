/***************************************************************************
                          16F.cc  -  description
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

#include "pic16F.hh"


pic16F::pic16F ()
{
}

/* -----------------------------------------------------------------------
   Program our RAM buffer into the PIC
   This routine programs each word and verify it immediatly
   but can be re-written to program ALL words, then verify them

   Already programmed words are NOT reprogrammed

   Returns:
   0 == OK
   1 == programm memory error
   2 == EEPROM data error
   4 == ID error
   8 == config fuses error

   the error codes are OR-ed
   ----------------------------------------------------------------------- */
int pic16F::program_pic (ostream& out)
{
    int stat, n;
    int x;

    stat = 0;
    unsigned int chip;

    initProgress ();
    
    // programming init sequence
    pulseEngine ("cdP,B,", 0);

    // Program the PIC's main program code memory
    for (x = memaddr1; x <= memaddr2; x++, --progress_status)
      {
        if (pulseEngine ("k4,F,V,", mem + x)) // don't program if already OK
	  {
            n = pulseEngine ("k2,S,k8w10000k4,F,V", mem + x);
            if (n != 0)
	      {
                stat |= 1;   // report failure
                pulseEngine ("?", &chip);
                warning (out, "Prog memory", x, chip, mem[x]);
	      }
	  }
        pulseEngine ("k6,"); // inc addr
      }

    // Program the EEPROM data area
    for (x = eeaddr1; x <= eeaddr2; x++, --progress_status)
      {
        if (pulseEngine ("k5,f,v", mem + x))
	  {
            n = pulseEngine ("k3,S,k8w10000k5,f,v,", mem + x);
            if (n != 0)
	      {
                stat |= 2;   // report failure
                pulseEngine ("?", &chip);
                warning (out, "EEPROM data", x, chip, mem[x]);
	      }
	  }
        pulseEngine ("k6,"); // inc addr
      }

    // Program the ID configurations
    // enter configuration mode
    unsigned int dummy = 0x3FFF;
    pulseEngine ("k0,S,", &dummy);

    for (x = idaddr1; x <= idaddr2; x++, --progress_status)
      {
        n = pulseEngine ("k2,S,k8w10000k4,F,V", mem + x);
        if (n != 0)
	  {
            stat |= 4;   // report failure
            pulseEngine ("?", &chip);
            warning (out, "ID data", x, chip, mem[x]);
	  }
        pulseEngine ("k6,");
      }

    // program the fuses : no init needed, because we re-enter config mode
    n = pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,k2,S,k8w10000k4,F,V,",
		     mem + cfaddr1);
    if (n != 0)
      {
        stat |= 8;
        pulseEngine ("?", &chip);
        warning (out, "Config fuses", cfaddr1, chip, mem[cfaddr1]);
      }
    pulseEngine ("k6,"); // why ???

    --progress_status;
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdbp");

    return stat;
}

/* -----------------------------------------------------------------------
   Read the PIC's memory and compare it with RAM buffer.
   RAM buffer words equals to INVALID (0xFFFF) are considered as
   dummy and thus are NOT significant for the tests.

   Returns:
   0 == OK
   1 == programm memory error
   2 == EEPROM data error
   4 == ID error
   8 == config fuses error

   the error codes are OR-ed
   ----------------------------------------------------------------------- */
int pic16F::verify_pic (ostream& out)
{
    int status;
    int i;

    status = 0;
    // save mem buffer
    FragBuffer ram;
    mem.swap (ram);

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

    // compare EEPROM data
    for (i = eeaddr1; i <= eeaddr2; ++i)
      {
        if (mem[i] != ram[i] && ram[i] != INVALID)
	  {
            status |= 2;
            warning (out, "EEPROM", i, mem[i], ram[i]);
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
    int cf;
    for (cf = cfaddr1; cf <= cfaddr2; ++cf)
      {
        if (mem[cf] != ram[cf] && ram[cf] != INVALID)
	  {
            status |= 8;
            warning (out, "Config fuses", cf, mem[cf], ram[cf]);
	  }
      }

    // restore ram buffer
    mem.swap (ram);

    return status;
}

/* -------------------------------------------------------------------------
	Erase the pic
   ------------------------------------------------------------------------- */
int pic16F::erase_pic ()
{
    unsigned int word = 0x3FFF;

    pulseEngine ("cdP,B,k0,S,k6,k6,k6,k6,k6,k6,k6,k1,k7,k8,w8000,k1,k7;"
		 "k9Sk8w10000k11Sk8w10000;cdbp", &word);
    return 0;
}

/* -----------------------------------------------------------------------
         Read the PIC's memory into our RAM buffer
   ----------------------------------------------------------------------- */
void pic16F::read_pic ()
{
    int x;
    unsigned int word;

    fill_mem ();

    initProgress ();
    
    // general read initialization
    pulseEngine ("pbcdP,B,w1000", &word);

    // Read the PIC's FLASH program memory
    for (x = memaddr1; x <= memaddr2; x++, --progress_status)
        pulseEngine ("k4,R,k6,", mem + x);

    // Read Pic's EEPROM memory
    for (x = eeaddr1; x <= eeaddr2; x++, --progress_status)
        pulseEngine ("k5,r,k6,", mem + x);

    // Read ID data (application specific)
    // only the 4 lsb of each word are significants

    pulseEngine ("k0Sw10", &word);  // goto 2000H

    for (x = idaddr1; x <= idaddr2; x++, --progress_status)
      pulseEngine ("k4,R,k6,", mem + x);

    // read fuses
    pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,", &word);
    for (x = cfaddr1; x <= cfaddr2; ++x, --progress_status)
      pulseEngine ("k4,R,k6,", &mem[x]);

    // power off sequence
    pulseEngine ("cdpb");
}

/* -----------------------------------------------------------------------
   Read the device ID (addr 0x2006)
   
   Parameters:
   id raw (unmasked) device ID

   Returns:
   true if applicable (old devices can return false)
   ----------------------------------------------------------------------- */
unsigned int pic16F::get_deviceID ()
{
    unsigned int id = static_cast<unsigned int>(~0);
    pulseEngine ("pbcd,P,w100,B,w100,k0,S,k6,k6,k6,k6,k6,k6,k4,R,cdpb,w10000", &id);
    return id & wmask ();
}
