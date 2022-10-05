/***************************************************************************
                     generic10f20x_12f50x.cc  -  description
                             -------------------
    begin                : Sun Jan 15 2006
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

#include <iostream>
#include <cstdio>
using namespace std;

#include "generic10F20x_12F50x.hh"


generic10F20x_12F50x::generic10F20x_12F50x ()
{
  // nothing to do : will be performed by picFactory::getPic ()
}

/* -----------------------------------------------------------------------
   Program our RAM buffer into the PIC

   Parametrized version

   Returns:
   0 == OK
   1 == programm memory error
   2 == EEPROM data error (never used for 16F5x)
   4 == ID error
   8 == config fuses error

   the error codes are OR-ed
   ----------------------------------------------------------------------- */
int generic10F20x_12F50x::program_pic (ostream& out)
{
    int stat;
    int x;
    int top_of_memory;
    int bottom_of_memory;
    unsigned int saved_osccal, saved_backup_osccal;
    unsigned int needed_osccal, needed_backup_osccal;

    stat = 0;

    initProgress ();
    
    // TODO : clean this code !!
    top_of_memory = memaddr2;
    if (top_of_memory < idaddr2)
      top_of_memory = idaddr2;
    if ((top_of_memory < osccal_location)
	&& (osccal_location != -1))
      top_of_memory = osccal_location;
    if ((top_of_memory < calibration1)
	&& (calibration1 != -1))
      top_of_memory = calibration1;

    bottom_of_memory = memaddr1;
    if (bottom_of_memory > idaddr1)
      bottom_of_memory = idaddr1;
    if ((bottom_of_memory > osccal_location)
	&& (osccal_location != -1))
      bottom_of_memory = osccal_location;
    if((bottom_of_memory > calibration1)
       && (calibration1 != -1))
      bottom_of_memory = calibration1;

    if ((osccal_location != -1) || (calibration1 != -1))
      {
        // After entering programming mode, PC points to Config fuses
        // increment adress  (code == 6) instruction rolls PC to 0x000
        pulseEngine ("cdPw5Bw5", 0);
        for (x = bottom_of_memory; x <= top_of_memory; x++)
	  {
            pulseEngine ("k6,");
            if (x == osccal_location)
	      pulseEngine ("k4,R,", &saved_osccal);
            if (x == calibration1)
	      pulseEngine ("k4,R,", &saved_backup_osccal);
	  }
        pulseEngine (",b,p,c,d,", 0);
      }

    // as standard erasing does not erase calibration,
    // perform a brute force erasing
    // reenter programming mode
    pulseEngine ("cdPw5Bw5", 0);
    // After entering programming mode, PC points to Config fuses
    // increment adress  (code == 6) instruction rolls PC to 0x000
    pulseEngine ("k6,", 0);
    // Increment address until it points to user ID locations
    for (x = memaddr1; x != idaddr1; ++x)
      pulseEngine ("k6,", 0);
    // Send bulk erase command (code == 9) and wait 10mS
    pulseEngine ("k9w10000", 0);
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine (",b,p,c,d,", 0);

    // prepare needed values for oscal & backup ostcal, as needed by programming option
    if (programming_option == 0)
      {
        needed_osccal = saved_osccal;
	needed_backup_osccal = saved_backup_osccal;
      }
    else if (programming_option == 1)
      {
        needed_osccal = mem[osccal_location];
        needed_backup_osccal = saved_backup_osccal;
      }
    else if (programming_option == 2)
      {
        needed_osccal = mem[osccal_location];
        needed_backup_osccal = mem[osccal_location];
      }
    else if (programming_option == 3)
      {
        needed_osccal = saved_backup_osccal;
        needed_backup_osccal = saved_backup_osccal;
      }

    pulseEngine ("cdPw5Bw5", 0);
    // Program the PIC's main program code memory
    for (x = bottom_of_memory; x <= top_of_memory; ++x, --progress_status)
      {
        pulseEngine ("k6,", 0);
        if (x == osccal_location)
	  {
	    if (pulseEngine ("k2,S,k8w2000k14w100k4,F,V", &needed_osccal) != 0)
	      {
		stat |= 1;
		warning (out, "OSCCAL location", x);
	      }
	  }
        else if (x == calibration1)
	  {
	    if (pulseEngine ("k2,S,k8w2000k14w100k4,F,V", &needed_backup_osccal) != 0)
	      {
		stat |= 4;
		warning (out, "Backup OSCCAL location", x);
	      }
	  }
        else if ((x >= memaddr1) && (x <= memaddr2))
	  {
            if (pulseEngine ("k2,S,k8w2000k14w100k4,F,V", mem + x) != 0)
	      {
                stat |= 1;
                warning (out, "Prog memory", x);
	      }
	  }
        else
        if ((x >= idaddr1) && (x <= idaddr2))
	  {
            if (pulseEngine ("k2,S,k8w2000k14w100k4,F,V", mem + x) != 0)
	      {
                stat |= 4;
                warning (out, "ID data", x);
	      }
	  }
      }
    pulseEngine (",b,p,c,d,", 0);

    pulseEngine ("cdPw5Bw5", 0);
    // After entering programming mode, PC points to Config fuses
    if (mem[cfaddr1] != INVALID)
      {
	if (pulseEngine ("k2,S,k8w2000k14w100k4,F,V", mem + cfaddr1) != 0)
	  {
	    warning (out, "Config fuses", cfaddr1);
	    stat |= 8;
	  }
      }
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine (",b,p,c,d,", 0);

    return stat;
}

/* -----------------------------------------------------------------------
   Read the PIC's memory into our RAM buffer
   ----------------------------------------------------------------------- */
void generic10F20x_12F50x::read_pic ()
{
    int x;

    fill_mem (); // fill progr buffer with INVALID pattern

    initProgress ();

    // general read initialization
    pulseEngine ("cdPw5Bw5");

    // read Config Fuses
    pulseEngine ("k4,R,", mem + cfaddr1);
    --progress_status;
    // Read the PIC's  program memory  + ID
    for (x = memaddr1; x <= idaddr2; x++, --progress_status)
      pulseEngine ("k6,k4,R,", mem + x);

    // power off sequence
    pulseEngine (",b,p,c,d,");

    // read calibration  values, if any
    if (calibration1 != -1)
      {
	// reenter programming mode
	pulseEngine ("cdPw5Bw5");
	// goto calibration word
	for (x = 0; x < calibration1; x++)
	  pulseEngine ("k6,");
	// read calibration words
	for (x = calibration1; x <= calibration2; x++)
	  pulseEngine ("k6,k4,R", mem + x);
	// power off sequence
	pulseEngine (",b,p,c,d,");
      }
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
int generic10F20x_12F50x::verify_pic (ostream& out)
{
    int status;
    int i;

    status = 0;

    FragBuffer ram;
    mem.swap (ram); // exchange buffer, leave adresses in mem inchanged
    read_pic ();

    // compare  program memory
    for (i = memaddr1; i <= memaddr2; ++i)
      if ((i != calibration1) && (i != calibration2))
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
    if (cfaddr1 != cfaddr2)
      if (mem[cfaddr2] != ram[cfaddr2] && ram[cfaddr2] != INVALID)
	{
	  status |= 8;
	  warning (out, "Config fuses", cfaddr2, mem[cfaddr2], ram[cfaddr2]);
	}

    // restore ram buffer
    mem.swap (ram);

    return status;
}

/* -------------------------------------------------------------------------
   Erase the pic
   ------------------------------------------------------------------------- */
int generic10F20x_12F50x::erase_pic ()
{
    unsigned int save_osccal, save_bosccal;
    int x;
    int stat;

    stat = 0;

    if (osccal_location != -1)
      {
        // programming init sequence
        pulseEngine ("cdPw5Bw5", 0);
        // read OSCCAL value
        for (x = memaddr1; x <= osccal_location; x++)
	  pulseEngine ("k6,"); // goto last word
        pulseEngine ("k4,R,", &save_osccal);
        // shutdown sequence VPP off, VDD off, etc..
        pulseEngine (",b,p,c,d,", 0);
      }

    if (calibration1 != -1)
      {
        // programming init sequence
        pulseEngine ("cdPw5Bw5", 0);
        // read backup OSCCAL value
        for (x = memaddr1; x <= calibration1; x++)
	  pulseEngine ("k6,"); // skip past ID locations
        pulseEngine ("k4,R,", &save_bosccal);
        // shutdown sequence VPP off, VDD off, etc..
        pulseEngine (",b,p,c,d,", 0);
      }

    // reenter programming mode
    pulseEngine ("cdPw5Bw5", 0);
    // After entering programming mode, PC points to Config fuses
    // increment adress  (code == 6) instruction rolls PC to 0x000
    pulseEngine ("k6,", 0);
    // Increment address until it points to user ID locations
    for (x = memaddr1; x != idaddr1; ++x)
      pulseEngine ("k6,", 0);
    // Send bulk erase command (code == 9) and wait 10mS
    pulseEngine ("k9w10000", 0);
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine (",b,p,c,d,", 0);

    if (osccal_location != -1)
      {
        // reenter programming mode
        pulseEngine ("cdPw5Bw5", 0);
        // goto last word
        for (int x = memaddr1; x <= osccal_location; x++)
	  pulseEngine ("k6,");
        // reprogram OSCCAL
        if (pulseEngine ("k2,S,k8w2000k14w100k4,F,V", &save_osccal) !=0)
	  stat |= 8;
        pulseEngine (",b,p,c,d,", 0);
      }

    if (calibration1 != -1)
      {
        // reenter programming mode
        pulseEngine ("cdPw5Bw5", 0);
        // skip past ID locations
        for (int x = memaddr1; x <= calibration1; x++)
	  pulseEngine ("k6,");
        // reprogram backup OSCCAL
        if (pulseEngine ("k2,S,k8w2000k14w100k4,F,V", &save_bosccal) !=0)
	  stat |= 8;
        pulseEngine (",b,p,c,d,", 0);
      }

    return stat;
}

const char *generic10F20x_12F50x::names ()
{
    return "p10f200 p10f202 p10f204 p10f206 p12f508 p12f509 p16f505 p16f506 p12f510";
}

void generic10F20x_12F50x::postInstanciate (const char *)
{
    calibration1 = calibration2 = idaddr2 + 1;
    osccal_location = memaddr2;
}

const char *generic10F20x_12F50x::SPLocs ()
{
    static char buf[200];
    sprintf (buf, "%X{OSCCAL word}%X{Backup OSCCAL word}", osccal_location, calibration1);
    return buf;
}

const char *generic10F20x_12F50x::progOptions ()
{
    return "{Do not program OSCCAL nor backup OSCCAL}"
           "{Program OSCCAL, leave backup OSCCAL untouched}"
           "{Program both OSCCAL and backup OSCCAL with OSCCAL value}"
           "{Program OSCCAL with backup OSCCAL value}";
}
