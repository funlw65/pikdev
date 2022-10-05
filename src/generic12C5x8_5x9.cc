/***************************************************************************
                     generic12C5x8_5x9.cc  -  description
                             -------------------
    begin                : Tue Jan 17 2006
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

#include "generic12C5x8_5x9.hh"


generic12C5x8_5x9::generic12C5x8_5x9 ()
{
}

/* -----------------------------------------------------------------------
   Program our RAM buffer into the PIC

   Returns:
   0 == OK
   1 == programm memory error
   2 == EEPROM data error (never used for 12Cxxx)
   4 == ID error
   8 == config fuses error

   the error codes are OR-ed
   ----------------------------------------------------------------------- */
int generic12C5x8_5x9::program_pic (ostream& out)
{
    int stat, fail;
    int x;
    unsigned int osccal_save = 0;

    stat = 0;

    initProgress ();
    // programming init sequence
    pulseEngine ("cdP,Bw100", 0);

    // After entering programming mode, PC points to 0xFFF (Config fuses)
    if (mem[cfaddr1] != INVALID)
      {
        unsigned int safe_cf = mem[cfaddr1];
        // comment out the following line to authorize CF == 0
        // safe_cf |= 0x8;

        for (fail = 1; fail < 100; ++fail)
	  {
	    // conf fuses require a total of 10ms Pulses
            //  == 100 x 100 us
            if (pulseEngine ("k2,S,k8w100k14w5000k4,F,V", &safe_cf) == 0)
	      break;
	  }
        if (fail >= 100)
	  {
            stat |= 8;
            warning (out, "Config fuses", cfaddr1);
	  }
        else // 11 x fail additionnal passes
	  {
            for (fail *= 11; fail > 0; --fail)
	      pulseEngine ("k2w1Sw1k8w100k14w5000", &safe_cf);
	  }
      }
    --progress_status;

    // increment address (code == 6) instruction rolls PC from 0xFFF to 0x000
    //
    // Program the PIC's main program code memory + ID location
    // (this is possible because they are contiguous)
    if (programming_option == 0)
      {
        // do not reprogram
        osccal_save = mem[memaddr2];
        mem[memaddr2] = INVALID; // INVALID words are not programmed
      }
      
    for (x = memaddr1; x <= idaddr2; ++x, --progress_status)
      {
        pulseEngine ("k6,", 0);

        // TODO: memaddr2 contains clock calibration value and should be
        // reprogramed with its original value
        if (mem[x] != INVALID && pulseEngine ("k4,F,V", mem + x) != 0)
	  {
            for (fail = 1; fail <= 8; ++fail)
	      if (pulseEngine ("k2,S,k8w200k14w5000k4,F,V", mem + x) == 0)
		break;

            if (fail > 8)
	      {
                if (x >=idaddr1)
		  {
                    stat |= 4;
                    warning (out, "ID data", x);
		  }
                else
		  {
                    stat |= 1;
                    warning (out, "Prog memory", x);
		  }
	      }
            else // 11 x fail additionnal passes
	      {
		for (fail *= 11; fail > 0; --fail)
		  pulseEngine ("k2w1Sw1k8w100k14w5000", mem + x);
	      }
	  }
      }

    if (programming_option == 0)
      mem[memaddr2] = osccal_save;  // restore buffer

    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("w1bw1pw1", 0);

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
int generic12C5x8_5x9::verify_pic (ostream& out)
{
    int status;
    int i;

    status = 0;
    // save ram buffer
    FragBuffer ram;
    mem.swap (ram); // exchange buffers, leave adresses in mem inchanged

    read_pic ();

    // compare  program memory
    for (i = memaddr1; i <= memaddr2; ++i)
      if (mem[i] != ram[i] && ram[i] != INVALID)
	{
	  status |= 1;
	  warning (out, "Prog memory", i, mem[i], ram[i]);
        }

    // compare ID
    for (i = idaddr1; i <= idaddr2; ++i)
      if (mem[i] != ram[i] && ram[i] != INVALID)
	{
	  status |= 4;
	  warning (out, "ID data", i, mem[i], ram[i]);
        }

    // compare config fuses
    if (mem[cfaddr1] != ram[cfaddr1] && ram[cfaddr1] != INVALID)
      {
        status |= 8;
        warning (out, "Config fuses", cfaddr1, mem[cfaddr1], ram[cfaddr1]);
      }

    // restore ram buffer
    mem.swap (ram);

    return status;
}

/* -------------------------------------------------------------------------
        Erase the pic
	Always return 16 (non applicable)
 	Because 12C508 is only UV erasable
// ------------------------------------------------------------------------- */
int generic12C5x8_5x9::erase_pic ()
{
    return 16;
}

/* -----------------------------------------------------------------------
        Read the PIC's memory into our RAM buffer
   ----------------------------------------------------------------------- */
void generic12C5x8_5x9::read_pic ()
{
    int x;
    unsigned int word;

    fill_mem (); // fill progr buffer with INVALID pattern

    initProgress ();

    // general read initialization
    pulseEngine ("cdPw1Bw1", &word);

    // read Config Fuses
    pulseEngine ("k4w1Rw1", mem + cfaddr1);
    --progress_status;

    // Read the PIC's program memory + ID
    for (x = memaddr1; x <= idaddr2; x++, --progress_status)
      pulseEngine ("k6w1k4w1Rw1", mem + x);

    // power off sequence
    pulseEngine ("pw1bw1", 0);
}

const char *generic12C5x8_5x9::names ()
{
    return "p12c508 p12c508a p12ce518 p12c509 p12c509a p12ce519";
}

const char *generic12C5x8_5x9::SPLocs ()
{
    static char buf[100];
    sprintf (buf, "%X{OSCCAL word}", memaddr2);
    return buf;
}

const char *generic12C5x8_5x9::progOptions ()
{
    static char buf[100];
    sprintf (buf, "{Do not program OSCCAL word}{Program OSCCAL word with buffer data (0x%X)} ",
	     memaddr2);
    return buf;
}
