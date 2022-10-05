/***************************************************************************
                        genericDS39622.cc  -  description
                             -------------------
    begin                : Mon Jan 16 2006
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

#include "genericDS39622.hh"


genericDS39622::genericDS39622 ()
{
}

int genericDS39622::erase_pic ()
{
    pulseEngine ("cdpbw300000P,B");

    progress_status = 2;
    // general
    pulseEngine ("w100000k0;X0E3C;k0;X6EF8;k0;X0E00;k0;X6EF7;k0;X0E05;k0;X6EF6;k12;X0F0F;"
		 "k0;X0E3C;k0;X6EF8;k0;X0E00;k0;X6EF7;k0;X0E04;k0;X6EF6;k12;X8787;k0;X0000;");
    --progress_status;
    pulseEngine ("d,C,c,C,c,C,c,C,cw6000;X0000");
    --progress_status;

    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cdpb");
    return 0;
}

void genericDS39622::read_pic ()
{
    int x;
    unsigned int w, data;

    fill_mem ();

    // general read initialization
    pulseEngine ("cdpbw300000,P,Bw300000");

    // Read the PIC's FLASH program memory
    pulseEngine ("w300000k0,X0E00,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6w1000");
    

    initProgress ();
    for (x = memaddr1; x <= memaddr2; x++, --progress_status)
      pulseEngine ("w10R", &mem[x]);
    pulseEngine ("cdw300000");

    // Read Pic's EEPROM memory : this part differs from previous pic18 devicess
    int xb;
    pulseEngine ("k0,X9EA6,k0,X9CA6,");
    for (x = eeaddr1, xb = 0; x <= eeaddr2; ++x, --progress_status)
      {
        // low byte
        w = 0x0E00 | (xb & 0xFF);
        pulseEngine("k0,S,k0,X6EA9", &w); // low part of addr
        w = 0x0E00 | ((xb >> 8) & 0xFF);
        pulseEngine ("k0,S,k0,X6EAA", &w); // hi part of addr

        pulseEngine ("k0,X80A6,k0,X50A8,k0,X6EF5,k0,X0000,r", &data);
        mem[x] = data;
        ++xb;

        // hi byte
        w = 0x0E00 | (xb & 0xFF);
        pulseEngine ("k0,S,k0,X6EA9", &w); // low part of addr
        w = 0x0E00 | ((xb >> 8) & 0xFF);
        pulseEngine ("k0,S,k0,X6EAA", &w); // hi part of addr

        pulseEngine ("k0,X80A6,k0,X50A8,k0,X6EF5,k0,X0000,r", &data);
        mem[x] |= data << 8;
        ++xb;
      }

    // Read Pic's Configuration bits memory
    pulseEngine ("w300000k0,X0E30,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6,w1000");

    for (x = cfaddr1; x <= cfaddr2; x++, --progress_status)
      pulseEngine ("w10R", &mem[x]);

    // Read Pic's ID bits memory
    pulseEngine ("w300000k0,X0E20,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6,w1000");

    for (x = idaddr1; x <= idaddr2; x++, --progress_status)
      pulseEngine ("w10R", &mem[x]);

    pulseEngine ("cdpb");
}

int genericDS39622::program_pic (ostream& out)
{
    unsigned int data, w;
    int x, timeout;
    int eex;

    erase_pic ();

    // enter Hi Voltage mode
    pulseEngine ("cdpbP,B");
    // Memory code.

    // source: DS39622E, p16
    // step 1
    pulseEngine ("w300000k0,X8EA6,k0,X9CA6,");

    /* Do not uncomment
       data = 0x0E00 | ((x >> 16) & 0xFF);
       pulseEngine ("k0,S,k0,X6EF8,", &data);
       data = 0x0E00 | ((x >> 8) & 0xFF);
       pulseEngine ("k0,S,k0,X6EF7,", &data);
       data = 0x0E00 | ((x) & 0xFF);
       pulseEngine ("k0,S,k0,X6EF6,", &data); */
    pulseEngine ("k0,X0E00,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6,");

    int kbf;

    initProgress ();
    
    for (x = memaddr1; x <= memaddr2; x += wr_buf_size, progress_status -= wr_buf_size)
      {
        // step 2
        for (kbf = 0; kbf < wr_buf_size - 1; ++kbf)
	  pulseEngine ("k13,S,", &mem[x + kbf]);

        pulseEngine ("k14,S,d,C,c,C,c,C,c,C,w2000,c,w200,X0000,", &mem[x + kbf]);
      }
    // end of memory code programming

    // EEProm DS39622E page 25, 26
    // step 1
    if (eeaddr1 != -1)
      {
	pulseEngine ("k0,X9EA6,k0,X9CA6,");

	for (x = eeaddr1, eex = 0; x <= eeaddr2; ++x, --progress_status)
	  {
	    // We have to do the job twice because
	    // 2 bytes are packed in each word of hexfile !

	    // lower part of the word
	    // step 2
	    w = (eex & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA9,", &w); // send address L
	    w = ((eex >> 8) & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EAA,", &w); // send address H
	    // step 3
	    data = (mem[x]) & 0xFF; // send L byte
	    w = data | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA8,", &w);
	    // step 4
	    pulseEngine ("k0,X84A6,");
	    // step 5
	    pulseEngine ("k0,X82A6,");
	    // step 6 & 7
	    // if something goes wrong, this loop might be infinite
	    // timeout avoid such a situation
	    timeout = 15;
	    do
	      {
		pulseEngine ("k0,X50A6,k0,X6EF5,k0,X0000,r,", &w);
	      }
	    while (--timeout && (w & 2));

	    pulseEngine ("cw200");
	    // step 8
	    pulseEngine ("k0,X94A6,");
	    ++eex;

	    // upper part of the word
	    // step 2
	    w = (eex & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA9,", &w); // send address L
	    w = ((eex >> 8) & 0xFF) | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EAA,", &w); // send address H
	    // step 3
	    data = (mem[x] >> 8) & 0xFF;  // send H byte
	    w = data | 0x0E00;
	    pulseEngine ("k0,S,k0,X6EA8,", &w);
	    // step 4
	    pulseEngine ("k0,X84A6,");
	    // step 5
	    pulseEngine ("k0,X82A6,");
	    // step 6 & 7
	    timeout = 15;
	    do
	      {
		pulseEngine ("k0,X50A6,k0,X6EF5,k0,X0000,r,", &w);
	      }
	    while (--timeout && (w & 2));

	    pulseEngine ("cw200");
	    // step 8
	    pulseEngine ("k0,X94A6,");
	    ++eex;
	  }
      }

    // ID programming DS39622E p 21

    // step 1
    pulseEngine ("w300000k0,X8EA6,k0,X9CA6,");

    // step 2
    pulseEngine ("k0,X0E20,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,X0E00,k0,X6EF6,");
    int k = idaddr1;
    pulseEngine ("k13,S", &mem[k]); ++k;
    pulseEngine ("k13,S", &mem[k]); ++k;
    pulseEngine ("k13,S", &mem[k]); ++k;
    pulseEngine ("k15,S", &mem[k]);
    pulseEngine ("d,C,c,C,c,C,c,C,w2000,c,w200,X0000");

    // Config Bits programming DS39622E p22
    // step 1
    pulseEngine ("k0,X8EA6,k0,X8CA6,");
    // step 2
    for (x = cfaddr1, eex = 0; x <= cfaddr2; x++, --progress_status)
      {
        w = eex | 0x0E00; ++eex;
        pulseEngine ("k0,X0E30,k0,X6EF8,k0,X0E00,k0,X6EF7,k0,S,k0,X6EF6", &w);
        pulseEngine ("k15,S,d,C,c,C,c,C,c,C,w2000,c,w200,X0000", &mem[x]); // prog LSB
        w = eex | 0x0E00; ++eex;
        pulseEngine ("k0,S,k0,X6EF6", &w);
        pulseEngine ("k15,S,d,C,c,C,c,C,c,C,w2000,c,w200,X0000", &mem[x]); // prog MSB
      }

    // shutdown sequence VPP off, VDD off, etc..
    pulseEngine ("cd,b,p,");

    return verify_pic (out);
}

const char *genericDS39622::names ()
{
    return
    "p18f2221 p18f2321 p18f2410 p18f2420 p18f2450 p18f2455 p18f2480 p18f2410 "
    "p18f2515 p18f2520 p18f2525 p18f2550 p18f2580 p18f2585 p18f2610 p18f2620 p18f2680 "
    "p18f4221 p18f4321 p18f4410 p18f4420 p18f4450 p18f4455 p18f4480 p18f4510 p18f4515 "
    "p18f4520 p18f4525 p18f4550 p18f4580 p18f4585 p18f4610 p18f5620 p18f4680 "
    // new devices from DS39622K
    "p18f2221 p18f2423 p18f2458 p18f2510 p18f2523 p18f2553 "
    "p18f2682 p18f2685 p18f4221 p18f4223 p18f4458 "
    "p18f4523 p18f4553 p18f4620 p18f4682 p18f4685";
}
