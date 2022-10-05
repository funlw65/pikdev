/***************************************************************************
                    generic12C67x_CE67x.cc  -  description
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

#include "generic12C67x_CE67x.hh"


generic12C67x_CE67x::generic12C67x_CE67x ()
{
}

/* -----------------------------------------------------------------------
   Program our RAM buffer into the PIC

   Parametrized version

   Returns:
   0 == OK
   1 == programm memory error
   2 == EEPROM data error (never used for 12Cxxx)
   4 == ID error
   8 == config fuses error

   the error codes are OR-ed
   ----------------------------------------------------------------------- */
int generic12C67x_CE67x::program_pic (ostream& out)
{
    int stat, fail;
    int x;

    stat = 0;

    initProgress ();
    // programming init sequence
    pulseEngine ("cdP,Bw10", 0);

    // PC points to Addr 0
    // program memory
    for (x = memaddr1; x <= memaddr2; ++x, --progress_status)
      {
        if (mem[x] != INVALID && pulseEngine ("k4,F,V,", mem + x) != 0)
	  {
            for (fail = 1; fail <= 25; ++fail)
	      if (pulseEngine ("k2,S,k8w100k14,w5000k4,F,V,", mem + x) == 0)
		break;
            if (fail > 25)
	      {
                warning (out, "Prog memory", x);
                stat |= 1;
	      }
            else // 3 x fail additionnal passes
	      {
                for (fail *= 3; fail > 0; --fail)
		  pulseEngine ("k2w1Sw1k8w100k14w5000", mem + x);
	      }
	  }
        pulseEngine ("k6,", 0);  // next location
      }

    // enter configuration mode, PC points to 0x2000
    unsigned int dummy = wmask ();
    pulseEngine ("k0,S,", &dummy);

    // program ID locations
    for (x = idaddr1; x <= idaddr2; ++x, --progress_status)
      {
        if (mem[x] != INVALID)
	  {
            for (fail = 1; fail <= 25; ++fail)
            {
	      if (pulseEngine ("k2w1Sw1k8w100k14w5000k4w1Fw1V", mem + x) == 0)
		break;
            }
            if (fail >= 100)
	      {
                stat |= 4;
                warning (out, "ID data", x);
	      }
            else // 3 x fail additionnal passes
	      {
                for (fail *= 3; fail > 0; --fail)
		  pulseEngine ("k2w1Sw1k8w100k14w5000", mem + x);
	      }
	  }
        pulseEngine ("k6w1", 0);  // next location
      }

    // go to Config fuse location
    for (; x < cfaddr1; ++x)
      pulseEngine ("k6w1", 0);  // next location

    // must apply 100 programming cycle

    // I remove memory protection because it is irreversible
    // and can make a JW chip unusable (protection bit cannot be UV erased)
    unsigned int safe_cf = mem[cfaddr1];
    //safe_cf |= 0x3F60;

    for (fail = 1; fail <= 100; ++fail)
      pulseEngine ("k2w1Sw1k8w100k14w3000", &safe_cf);
    // verify
    if (pulseEngine ("k4w1Fw1V", &safe_cf) != 0)
      {
        warning (out, "Config fuses", cfaddr1);
        stat |= 8;
      }
    --progress_status;
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdw1bw1pw1", 0);

    return stat;
}

/* -----------------------------------------------------------------------
       Read the PIC's memory into our RAM buffer
   ----------------------------------------------------------------------- */

void generic12C67x_CE67x::read_pic ()
{
    int x;
    unsigned int word;

    fill_mem (); // fill progr buffer with INVALID pattern

    initProgress ();
    // general read initialization
    pulseEngine ("cdPw1Bw1", &word);

    // Read the PIC's program memory
    for (x = memaddr1; x <= memaddr2; x++, --progress_status)
      pulseEngine ("w1k4w1Rw1k6", mem + x);

    // enter configuration mode, PC points to 0x2000
    unsigned int dummy = wmask ();
    pulseEngine ("k0w1Sw1", &dummy);

    for (x = idaddr1; x <= idaddr2; x++, --progress_status)
      pulseEngine ("k4w1Rw1k6w1", mem + x);

    // go to Config fuse location
    for (; x < cfaddr1; ++x)
      pulseEngine ("k6w1", 0);

    // read Config Fuses
    pulseEngine ("k4w1Rw1", mem + cfaddr1);
    --progress_status;
    // power off sequence
    pulseEngine ("pw1bw1", 0);
}

const char *generic12C67x_CE67x::names ()
{
    return "p12c671 p12c672 p12ce673 p12ce674";
}

const char *generic12C67x_CE67x::SPLocs ()
{
    return "";
}

const char *generic12C67x_CE67x::progOptions ()
{
    return "";
}
