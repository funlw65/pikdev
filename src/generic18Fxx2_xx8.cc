/***************************************************************************
                      generic18Fxx2_xx8.cc  -  description
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

#include "generic18Fxx2_xx8.hh"


generic18Fxx2_xx8::generic18Fxx2_xx8 ()
{
}

int generic18Fxx2_xx8::verify_pic (ostream& out)
{
    FragBuffer ram;
    ram.swap (mem);  // leave mem adresses unchanged

    int status = 0, i;

    read_pic ();

    // compare program memory
    for (i = memaddr1; i <= memaddr2; ++i)
      {
        if (mem[i] != ram[i] && ram[i] != INVALID)
	  {
            status |= 1;
            warning (out, "Prog memory", i * 2, mem[i], ram[i]);
	  }
      }

    // compare EEPROM data
    if (eeaddr1 != -1)
      {
        for (i = eeaddr1; i <= eeaddr2; ++i)
          if (mem[i] != ram[i] && ram[i] != INVALID)
	    {
	      status |= 2;
	      warning (out, "EEPROM", i * 2, mem[i], ram[i]);
	    }
      }

    if (idaddr1 != -1)
      {
        // compare ID
	for (i = idaddr1; i <= idaddr2; ++i)
	  {
	    if (mem[i] != ram[i] && ram[i] != INVALID)
	    {
		status |= 4;
		warning (out, "ID data", i * 2, mem[i], ram[i]);
	    }
	  }
      }
    
    // compare config fuses
    for (i = cfaddr1; i <= cfaddr2; ++i)
      {
        if (mem[i] != ram[i] && ram[i] != INVALID)
	  {
            status |= 8;
            warning (out, "Config fuses", i * 2, mem[i], ram[i]);
	  }
      }

    // restore ram buffer
    mem.swap (ram); // restore old buffer

    return status;
}

int generic18Fxx2_xx8::program_pic (ostream& out)
{
    unsigned int data, w;
    int x;
    int eex;

    erase_pic ();

    progress_status = initProgress () + 1;
    // enter Hi Voltage mode
    pulseEngine ("cdpbP,B");
    // Memory code.

    // step 0 : direct access to config mem
    pulseEngine ("w300000,k0,X8EA6,k0,X8CA6,");

    // single panel programming technique, DS39576B page 12,13,14
    // step 1 & step 2 (1 panel write)

    pulseEngine ("w300000k0,X0E3C,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E06,k0,X6EF6,k12,X0000,");
    // step 3

    pulseEngine ("k0,X8EA6,k0,X9CA6,k0,X0E00,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6,");

    --progress_status;

    for (x = memaddr1; x <= memaddr2; x += wr_buf_size, progress_status -= wr_buf_size)
      {
        // step 4
        int kbf;
        for (kbf = 0; kbf < wr_buf_size - 1; ++kbf)
          pulseEngine ("k13,S,", &mem[x + kbf]);

        pulseEngine ("k15,S,", &mem[x + kbf]);

        pulseEngine ("d,C,c,C,c,C,c,Cw2000cw100X0000,"); // final sequence

        // "NULL" programming function, in fact increases
        // PICs memory address pointer
        pulseEngine ("k13,S,", &mem[x]);
      }
    // end of memory code programming

    // EEProm DS39576B page 16, 17
    // step 1
    if (eeaddr1 != -1)
      {
	pulseEngine ("k0,X9EA6,k0,X9CA6,");

	for (x = eeaddr1, eex = 0; x <= eeaddr2; ++x, --progress_status)
	  {
	    // We have to do the job twice because
	    // 2 bytes are packed in each word of hexfile !

	    // upper part of the word
	    // step 2
	    w = (eex & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA9,", &w); // send address
	    // step 3
	    data = (mem[x] >> 8) & 0xFF;  // send H byte
	    w = data | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA8,", &w);
	    // step 4
	    pulseEngine ("k0,X84A6,");
	    // step 5
	    pulseEngine ("k0,X0E55,k0,X6EA7,k0,X0EAA,k0,X6EA7,");
	    // step 6
	    pulseEngine ("k0,X82A6,");
	    // step 7
	    pulseEngine ("k0,X0000,k0,X0000w5005k0,X94A6,");
	    ++eex;

	    // lower part of the word
	    // step 2
	    w = (eex & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA9,", &w);
	    // step 3
	    data = mem[x] & 0xFF; //  send L byte
	    w = data | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA8,", &w);
	    // step 4
	    pulseEngine ("k0,X84A6,");
	    // step 5
	    pulseEngine ("k0,X0E55,k0,X6EA7,k0,X0EAA,k0,X6EA7,");
	    // step 6
	    pulseEngine ("k0,X82A6,");
	    // step 7
	    pulseEngine ("k0,X0000,k0,X0000w5005k0,X94A6,");
	    ++eex;
	  }
      }

    // Config Bits programming
    // step 1 & step 2 (1 panel write)

    for (x = cfaddr1, eex = 0; x <= cfaddr2; x++, --progress_status)
      {
        w = (eex & 0xFF) | 0x0E00;

        pulseEngine ("k0,X0E3C,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E06,k0,X6EF6,k12,X0000,k0,X8EA6,"
		     "k0,X8CA6,k0,X0E30,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,S,k0,X6EF6,", &w);
        pulseEngine ("k15,S,", &mem[x]);
        pulseEngine ("d,C,c,C,c,C,c,Cw2000cw10X0000,k0,X2AF6,");
        pulseEngine ("k15,S,", &mem[x]); // final sequence
        pulseEngine ("d,C,c,C,c,C,c,Cw2000cw10X0000"); // final sequence
        eex += 2;
      }
    // ID programming DS39576B page 18, 19
    // step 1 & step 2 (1 panel write)

    pulseEngine ("w300000k0,X0E3C,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E06,k0,X6EF6,k12,X0000,");
    // step 3

    pulseEngine ("k0,X8EA6,k0,X9CA6,k0,X0E20,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6,");

    for (x = idaddr1; x <= idaddr2; x += 4, progress_status -= 4)
      {
        pulseEngine ("k13,S,", &mem[x]);
        pulseEngine ("k13,S,", &mem[x + 1]);
        pulseEngine ("k13,S,", &mem[x + 2]);
        pulseEngine ("k15,S,", &mem[x + 3]);
        pulseEngine ("d,C,c,C,c,C,c,Cw2000cw10X0000,"); // final sequence
        pulseEngine ("k13,S,", &mem[x]);
        // "NULL  programming function, in fact increases
        // PICs memory address pointer
      }
    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cd,b,p,");

    return verify_pic (out);
}

void generic18Fxx2_xx8::read_pic ()
{
    int x;
    unsigned int w, L, H;

    fill_mem ();

    // general read initialization
    pulseEngine ("cdpbw300000,P,Bw300000");

    // Read the PIC's FLASH program memory
    pulseEngine ("w300000k0,X0E00,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6w1000");

    initProgress ();

    for (x = memaddr1; x <= memaddr2; x++, --progress_status)
      pulseEngine ("R", &mem[x]);
    pulseEngine ("cdw300000");

    // Read Pic's EEPROM memory
    int xb;
    pulseEngine ("k0,X9EA6,k0,X9CA6");
    for (x = eeaddr1, xb = 0; x != -1 && x <= eeaddr2; ++x, --progress_status)
      {
	w = 0x0E00 | (xb & 0xFF);
	pulseEngine ("k0,S,k0,X6EA9,k0,X80A6,k0,X50A8,k0,X6EF5,", &w);
	pulseEngine ("r", &H);
	++xb;
	w = 0x0E00 | (xb & 0xFF);
	pulseEngine ("k0,S,k0,X6EA9,k0,X80A6,k0,X50A8,k0,X6EF5,", &w);
	pulseEngine ("r", &L);
	++xb;
	mem[x] = (L | (H << 8));
      }

    // Read Pic's Configuration bits memory
    pulseEngine ("w300000k0,X0E30,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6");

    for (x = cfaddr1; x <= cfaddr2; x++, --progress_status)
      pulseEngine ("R", &mem[x]);

    // Read Pic's ID bits memory
    pulseEngine ("w300000k0,X0E20,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6");

    for (x = idaddr1; x <= idaddr2; x++, --progress_status)
      pulseEngine ("R", &mem[x]);

    pulseEngine ("cdpb");
}

int generic18Fxx2_xx8::erase_pic ()
{
    pulseEngine ("cdpbw300000P,B");

    pulseEngine ("w100000k0;X0E3C;k0;X6EF8;k0;X0E00;k0;X6EF7;"
		 "k0;X0E04;k0;X6EF6;k12;X0080;k0;X0000;k0;X0000w5005");

    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdpb");
    return 0;
}

const char *generic18Fxx2_xx8::names ()
{
    return
    "p18f242 p18f248 p18f252 p18f258 p18f442 p18f448 p18f452 p18f458 "
    "p18f1220 p18f1320 p18f2220 p18f2320 p18f4220 p18f4320";
}

unsigned int generic18Fxx2_xx8::get_deviceID ()
{
    unsigned int id = 0;
    // general read initialization
    pulseEngine ("cdpb,P,w10000,B,w10000");

    // Read the PIC's ID word (3FFFFE-3FFFFF)
    pulseEngine ("k0,X0E3F,k0,X6EF8,k0,X0EFF,k0,X6EF7,k0,X0EFE,k0,X6EF6,w1000,R,cdpb", &id);
    // I forge a pseudo ID with bit 15 forced to 1
    // because some 16F devices have the same ID as a 18F device.
    
    return id | 0x8000;
}
