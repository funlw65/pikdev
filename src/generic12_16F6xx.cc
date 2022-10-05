/***************************************************************************
                      generic12_16F6xx.cc  -  description
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
/* DS-41204F */

#include "generic12_16F6xx.hh"


generic12_16F6xx::generic12_16F6xx ()
{
}

const char *generic12_16F6xx::names ()
{
    return "p16f636 p16f639 p16f684 p16f685 p16f687 p16f688 p16f689 p16f690 p12f635 p12f683";
}

void generic12_16F6xx::read_pic ()
{
    int x;
    unsigned int word;

    fill_mem ();

    initProgress ();
    
    // general read initialization
    pulseEngine ("pbcdPw10,B,w1000", &word);


    // Read the PIC's FLASH program memory
    for (x = memaddr1; x <= memaddr2; x++, --progress_status)
      pulseEngine ("k4,R,k6,", mem + x);

    // Read Pic's EEPROM memory
    for (x = eeaddr1; x <= eeaddr2; x++, --progress_status)
      pulseEngine ("k5,r,k6,", mem + x);

    // Read ID data (application specific)
    // only the 4 lsb of each word are significants

    pulseEngine ("k0Sw10", &word); // goto 2000H

    for (x = idaddr1; x <= idaddr2; x++, --progress_status)
      pulseEngine ("k4,R,k6,", mem + x);

    // read fuses
    pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,", &word);
    for (x = cfaddr1; x <= cfaddr2; ++x, --progress_status)
      pulseEngine ("k4,R,k6,", &mem[x]);

    // read calibration words
    for (x = calibration1; x <= calibration2; ++x)
      pulseEngine ("k4,R,k6,", &mem[x]);

    // power off sequence
    pulseEngine ("cdpw10b");
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
   16 == calibration word error

   the error codes are OR-ed
   ----------------------------------------------------------------------- */
int generic12_16F6xx::program_pic (ostream& out)
{
    int stat, n;
    int x;

    stat = 0;
    unsigned int chip;

    // these devices have no erase/programming instruction
    // so they must be explicitly erased
    erase_pic ();

    initProgress ();
    // programming init sequence
    pulseEngine ("cdPw10,Bw10,", 0);

    // Program the PIC's main program code memory
    for (x = memaddr1; x <= memaddr2; x++, --progress_status)
      {
        if (pulseEngine ("k4,F,V,", mem + x)) // don't program if already OK
	  {
            n = pulseEngine ("k2,S,k8w10000k4,F,V", mem + x);
            if (n != 0)
	      {
                stat |= 1; // report failure
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
                stat |= 2; // report failure
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
    n = pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,k2,S,k8w10000k4,F,V,", mem + cfaddr1);
    if (n != 0)
      {
        stat |= 8;
        pulseEngine ("?", &chip);
        warning (out, "Config fuses", cfaddr1, chip, mem[cfaddr1]);
      }
    --progress_status;
    
    if (programming_option == 1)
      {
        // need to reprogram calibration word
        // I re-enter config mode to make pieces of code independant
        n = pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,k6,", &dummy);

        for (x = calibration1; x <= calibration2; ++x)
	  {
            n = pulseEngine ("k2,S,k8w10000k4,F,V,", mem + x);
            if (n != 0)
	      {
                stat |= 64;
                pulseEngine ("?", &chip);
                warning (out, "Calibration word", x, chip, mem[x]);
	      }
            pulseEngine ("k6,");
	  }
      }
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdpw10b");

    return stat;
}

const char *generic12_16F6xx::progOptions ()
{
    return "{Do not reprogram calibrations data}"
           "{Use hex file data to reprogram calibration data}";
}

int generic12_16F6xx::erase_pic ()
{
    unsigned int word;

    progress_status = 2;
    // The following algorithm comes from
    // DS41204E p13, section 3.14, second method
    // program Config word to 0 to clear /CPD bit
    word = 0;
    pulseEngine ("cdPw10,Bw10,");
    pulseEngine ("k0,S,k6,k6,k6,k6,k6,k6,k6,k2,S,k8w10000k4,F,V,w1000", &word);
    --progress_status;
    // now, erase prog memory and config word and user ID
    // note: as /CPD is cleared, data memory will also be erased
    pulseEngine ("k0,S,k9,w10000,cdpw10b", &word);
    --progress_status;

    return 0;
    // note: calibration data is never erased, but can be reprogrammed
    // without erasing
    // (for that, set programming_option to 1 using method setProgOption (int))
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
   64 == calibration word error

   the error codes are OR-ed
   ----------------------------------------------------------------------- */
int generic12_16F6xx::verify_pic (ostream& out)
{
    int status;
    int i;

    status = 0;
    // save mem buffer
    FragBuffer ram;
    mem.swap (ram);

    read_pic ();

    // compare program memory
    for (i = memaddr1; i != -1 && i <= memaddr2; ++i)
      {
        if (mem[i] != ram[i] && ram[i] != INVALID)
	  {
            status |= 1;
            warning (out, "Prog memory", i, mem[i], ram[i]);
	  }
      }

    // compare EEPROM data
    for (i = eeaddr1; i <= eeaddr2; ++i)
      if (mem[i] != ram[i] && ram[i] != INVALID)
	{
	  status |= 2;
	  warning (out, "EEPROM", i, mem[i], ram[i]);
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

    // compare calibration words
    for (i = calibration1; i != -1 && i <= calibration2; ++i)
      {
        if (mem[i] != ram[i] && ram[i] != INVALID)
	  {
            status |= 64;
            warning (out, "Calibration word", i, mem[i], ram[i]);
	  }
      }

    // restore ram buffer
    mem.swap (ram);

    return status;
}



