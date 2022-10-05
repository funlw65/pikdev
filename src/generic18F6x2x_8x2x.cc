/***************************************************************************
                     generic18F6x2x_8x2x.cc  -  description
                             -------------------
    begin           : Sun Jan 15 2006
    copyright       : (C) 2006 by Gibaud Alain and Jean-Claude Vandenhekke
    email           : alain.gibaud@free.fr
 ***************************************************************************/
/***************************************************************************
 17/12/2005 append by Jean-Claude Vandenhekke <jc.vdhekke@free.fr>

 15/01/06 Alain Gibaud: apply new generic framework to previous 18F6525
 class by j.c Vandenhekke.
 All devices covered by  DS30499B are now supported:

 NOT TESTED
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "generic18F6x2x_8x2x.hh"


generic18F6x2x_8x2x::generic18F6x2x_8x2x ()
{
    vr_mode = 0;
}

int generic18F6x2x_8x2x::verify_pic (ostream& out) // Ok
{
    FragBuffer ram;
    ram.swap (mem);  // leave mem adresses unchanged

    int status = 0, i;

    read_pic ();

    if (vr_mode == 0 || vr_mode == 1)
      {
        // compare  program memory
        for (i = memaddr1; i <= memaddr2; ++i)
	  {
            if (mem[i] != ram[i] && ram[i] != INVALID)
	      {
                status |= 1;
                warning (out, "Prog memory", i * 2, mem[i], ram[i]);
	      }
	  }

        // compare ID
        if (idaddr1 != -1)
	  {
	    for (i = idaddr1; i <= idaddr2; ++i)
	      {
		if (mem[i] != ram[i] && ram[i] != INVALID)
		  {
		    status |= 4;
		    warning (out, "ID data", i * 2, mem[i], ram[i]);
		  }
	      }
	  }
        // compare EEPROM data
        if (eeaddr1 != -1)
	  {
	    for (i = eeaddr1; i <= eeaddr2; ++i)
	      {
		if (mem[i] != ram[i] && ram[i] != INVALID)
		  {
		    status |= 2;
		    warning (out, "EEPROM", i * 2, mem[i], ram[i]);
		  }
	      }
	  }
      } // vr_mode 0 or 1

    if (vr_mode == 0 || vr_mode == 2)
      {
        // compare config fuses
        for (i = cfaddr1; i <= cfaddr2; ++i)
	  {
            if (mem[i] != ram[i] && ram[i] != INVALID)
	      {
                status |= 8;
                warning (out, "Config fuses", i * 2, mem[i], ram[i]);
	      }
	  }
      } // vr_mode 0 or 2

    // restore ram buffer
    mem.swap (ram); // restore old buffer

    return status;
}

int generic18F6x2x_8x2x::program_pic (ostream& out)
{
    unsigned int data, w;
    int x;
    int eex;

    erase_pic (); // Perform bulk erase    

    // enter  Hi Voltage mode
    pulseEngine ("cdpbw300000Pw10000Bw100000");

    // Algo see DS30499B page 6

    // 1 - Program Memory code.
    // single panel programming technique, DS30499B pages 15,16  Ok, non test
    // step 1 : direct access to config mem Ok
    pulseEngine ("k0,X8EA6,k0,X8CA6,");

    // step 2 : configure device for single panel write
    // Write 00h to 3C0006h to enable single panel write.
    pulseEngine ("k0,X0E3C,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E06,k0,X6EF6,k12,X0000,");

    // step 3 : Direct access to code memory
    pulseEngine ("k0,X8EA6,k0,X9CA6,");

    // Step 4 to 8 remplaced by bulk erase.

    // step 9a : start address 000000h
    pulseEngine ("k0,X0E00,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6,");

    initProgress ();
    
    for (x = memaddr1; x <= memaddr2; x += wr_buf_size, progress_status -= wr_buf_size)
      {
        // step 9b
        int kbf;
        for (kbf = 0; kbf < wr_buf_size - 1; ++kbf)
	  pulseEngine ("k13,S,", &mem[x + kbf]);

        pulseEngine ("k15,S,", &mem[x + kbf]);

        // final sequence, eq ("k0,X0000")
        pulseEngine ("d,C,c,C,c,C,c,Cw2000cw100X0000,");

        // "NULL" programming function, in fact increases
        // PICs memory address pointer
        pulseEngine ("k13,S,", &mem[x]);
      }
    // end of memory code programming
    pulseEngine ("w300000");

    // 2 - ID programming, see DS39576B page  18,19
    // step 1 & step 2   (1 panel write)
    pulseEngine ("k0,X8EA6,k0,X8CA6,k0,X0E3C,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E06,k0,X6EF6,k12,X0000,");
    // step 3
    pulseEngine ("k0,X8EA6,k0,X9CA6,k0,X0E20,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6,");

    for (x = idaddr1; x <= idaddr2; x += 4, progress_status -= 4)
      {
        pulseEngine ("k13,S,", &mem[x]);
        pulseEngine ("k13,S,", &mem[x + 1]);
        pulseEngine ("k13,S,", &mem[x + 2]);
        pulseEngine ("k15,S,", &mem[x + 3]);
        // final sequence, eq ("k0,X0000")
        pulseEngine ("d,C,c,C,c,C,c,Cw2000cw10X0000,");
        // "NULL  programming function, in fact increases
        // PICs memory address pointer
        pulseEngine ("k13,S,", &mem[x]);
      }

    // 3 - EEProm programming, DS30499B page 17, 18
    // step 1, direct access to data EEPROM
    if (eeaddr1 != -1)
      {
	pulseEngine ("k0,X9EA6,k0,X9CA6,");

	for (x = eeaddr1, eex = 0; x <= eeaddr2; ++x, --progress_status)
	  {
	    // We have to do the job twice because
	    // 2 bytes are packed in each word of hexfile !

	    // 1 -s upper part of the word
	    // step 2, Set the data EEPROM address pointer
	    w = (eex & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA9,", &w);
	    w = ((eex >> 8) & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EAA,", &w);
	    // step 3, Load the data to be written
	    data = (mem[x] >> 8) & 0xFF; // send H byte
	    w = data | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA8,", &w);
	    // step 4, Enable memory write
	    pulseEngine ("k0,X84A6,");
	    // step 5, Perform required sequence
	    pulseEngine ("k0,X0E55,k0,X6EA7,k0,X0EAA,k0,X6EA7,");
	    // step 6, Innitiate write
	    pulseEngine ("k0,X82A6,");
	    // step 7
	    pulseEngine ("k0,X0000,k0,X0000w5005,"); // ????? Ok
	    // step 8, disable write
	    pulseEngine ("k0,X94A6,");
	    ++eex;

	    // 2 - lower part of the word
	    // step 2
	    w = (eex & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA9,", &w);
	    w = ((eex >> 8) & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EAA,", &w);
	    // step 3
	    data = mem[x] & 0xFF; // send L byte
	    w = data | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA8,", &w);
	    // step 4
	    pulseEngine ("k0,X84A6,");
	    // step 5
	    pulseEngine ("k0,X0E55,k0,X6EA7,k0,X0EAA,k0,X6EA7,");
	    // step 6
	    pulseEngine ("k0,X82A6,");
	    // step 7
	    pulseEngine ("k0,X0000,k0,X0000w5005,"); // ????? Ok
	    // step 8, disable write
	    pulseEngine ("k0,X94A6,");
	    ++eex;
	  }
      }
    
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdb,p");

    // 4 - Verify pic programming, flash + eeprom
    vr_mode = 1;
    int result = verify_pic (out);
    vr_mode = 0;
    if (result != 0)
      {
        // shutdown sequence VPP off, VDD off, etc..
        pulseEngine ("cdb,p");
        return result;
      }

    // enter Hi Voltage mode
    pulseEngine ("cdpbw300000Pw10000Bw100000");

    // Algo see DS30499B page 20

    // 5 - Config Bits programming
    // (1 panel write)
    pulseEngine ("k0,X8EA6,k0,X8CA6,k0,X0E3C,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E06,k0,X6EF6,k12,X0000,");
    for (x = cfaddr1, eex=0; x <= cfaddr2; x++, --progress_status)
      {
        // Step 1 Direct acces to config memory
        pulseEngine ("k0,X8EA6,k0,X8CA6,");
        // Step 2 Position the program counter, goto 100000h (hors champ)
        pulseEngine ("k0,XEF00,k0,XF800,");
        // Step 3a Set table pointer for config byte to be written.
        w = (eex & 0xFF) | 0x0E00;
        pulseEngine ("k0,X0E30,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,S,k0,X6EF6,", &w);
        // Step 3b Write even/odd address
        pulseEngine ("k15,S,", &mem[x]); // LSB
        pulseEngine ("d,C,c,C,c,C,c,Cw2000cw10X0000,k0,X2AF6,");
        pulseEngine ("k15,S,", &mem[x]); // MSB
        pulseEngine ("d,C,c,C,c,C,c,Cw2000cw10X0000");
        // Step 4 Execute four NOPs
        pulseEngine ("k0,X0000,k0,X0000,k0,X0000,k0,X0000,");
        eex += 2;
      }

    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdb,p");

    // 6 - Verify config programming
    vr_mode = 2;
    result = verify_pic (out);
    vr_mode = 0;

    return result;
}

void generic18F6x2x_8x2x::read_pic () // Ok
{
    int x;
    unsigned int w, L, H;

    fill_mem ();

    // computation of number of steps is rather complicated ..
    int steps = 0;    
    if (vr_mode == 0 || vr_mode == 1)
      steps += ((memaddr2 - memaddr1 + 1) + (idaddr2 - idaddr1 + 1));
    if (vr_mode == 0 || vr_mode == 2)
      steps += (cfaddr2 - cfaddr1 + 1);
    if (vr_mode == 0 || vr_mode == 1)
      steps += (eeaddr2 - eeaddr1 + 1);
    progress_status = steps;   

    // general read initialization
    pulseEngine ("cdpbw300000Pw10000Bw100000");

    if (vr_mode == 0 || vr_mode == 1)
      {
        // Read the PIC's FLASH program memory
        pulseEngine ("k0,X0E00,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6");
        for (x = memaddr1; x <= memaddr2; x++, --progress_status)
	  pulseEngine ("w1000R", &mem[x]);
        pulseEngine ("cdw100000");

        // Read Pic's ID bits memory, addr : 0x200000
        pulseEngine ("k0,X0E20,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6");
        for (x = idaddr1; x <= idaddr2; x++, --progress_status)
	  pulseEngine ("w1000R", &mem[x]);
        pulseEngine ("cdw100000");
      } // vr_mode

    if (vr_mode == 0 || vr_mode == 2)
      {
        // Read Pic's Configuration bits memory, addr : 0x300000
        pulseEngine ("k0,X0E30,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6");
        for (x = cfaddr1; x <= cfaddr2; x++, --progress_status)
	  pulseEngine ("w1000R", &mem[x]);
        pulseEngine ("cdw100000");
      } // vr_mode

    if (vr_mode == 0 || vr_mode == 1)
      {
        // Read Pic's EEPROM memory
        int xb;
        // Direct access to data eeprom
        pulseEngine ("k0,X9EA6,k0,X9CA6");
        for (x = eeaddr1, xb = 0; x <= eeaddr2; ++x, --progress_status)
	  {
            w = 0x0E00 | (xb & 0xFF);
            pulseEngine ("k0,S,k0,X6EA9,", &w);
            w = 0x0E00 | ((xb >> 8) & 0xFF);
            pulseEngine ("k0,S,k0,X6EAA,", &w);
            pulseEngine ("k0,X80A6,k0,X50A8,k0,X6EF5,r", &H);
            ++xb;
            w = 0x0E00 | (xb & 0xFF);
            pulseEngine ("k0,S,k0,X6EA9,", &w);
            w = 0x0E00 | ((xb >> 8) & 0xFF);
            pulseEngine ("k0,S,k0,X6EAA,", &w);
            pulseEngine ("k0,X80A6,k0,X50A8,k0,X6EF5,r", &L);
            ++xb;
            mem[x] = (L | (H << 8));
	  }
        pulseEngine ("cdw30000");
      } // vr_mode

    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdb,p");
}

int generic18F6x2x_8x2x::erase_pic ()
{
    progress_status = 2;
    pulseEngine ("cdpbw300000Pw10000Bw100000");

    --progress_status;
    // bulk chip erase command sequence ( P10 + P11 = 5005 µS )
    pulseEngine ("k0,X0E3C,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E04,k0,X6EF6,"
                 "k12,X0080,k0,X0000,k0,X0000w5005");

    --progress_status;
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdb,p");
    return 0;
}

const char *generic18F6x2x_8x2x::names ()
{
    return "p18f6525 p18f6621 p18f8525 p18f8621";
}

unsigned int generic18F6x2x_8x2x::get_deviceID ()
{
    // TODO : This code is common to ALL 18F - any factorization ?
    unsigned int id = 0;
    // general read initialization
    pulseEngine ("cdpbPw10000Bw100000");

    // Read the PIC's ID word    (3FFFFE-3FFFFF)
    pulseEngine ("k0,X0E3F,k0,X6EF8,k0,X0EFF,k0,X6EF7,k0,X0EFE,k0,X6EF6,R,", &id);

    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdb,p");
    // I forge a pseudo ID with bit 15 forced to 1
    // because some 16F devices have the same ID as a 18F device.
    return id | 0x8000;
}
