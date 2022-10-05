/***************************************************************************
                          pic18F.cc  -  description
                             -------------------
    begin                : Mon Jun 7 2003
    copyright            : (C) 2003 by Alain Gibaud
    email                : alain.gibaud@univ-valenciennes.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "pic18F.hh"


pic18F::pic18F ()
{
    clk_delay = 0;
}

pic18F::~pic18F ()
{
}

void pic18F::send_data (unsigned int d)
{
    int x;

    //Keep the write state...
    hard_->rwwrite ();

    for (x = 0; x < 16; x++)
      {
        hard_->clkhi ();
        if (d & 0x01)
	  hard_->outhi (); // Data bit is a one
        else
	  hard_->outlo (); // Data bit is a zero
        waitus (clk_delay + 5);
        hard_->clklo ();
        waitus (clk_delay + 3);
        d >>= 1; // Move the data over 1 bit
      }
    hard_->outhi ();

    // Normal (safe) state...
    hard_->rwread ();
}

unsigned int pic18F::get_word ()
{
    unsigned int x, ind = 0;

    // Normal (safe) state...
    hard_->rwread ();

    send_cmd (9);
    hard_->outhi ();
    for (x = 0; x < 16; x++)
      {
        hard_->clkhi ();
        waitus (clk_delay + 5);
        if (x > 7)
	  if (hard_->inbit ())
	    ind |= (1 << (x - 8));
        hard_->clklo ();
        waitus (clk_delay + 3);
      }
    send_cmd (9);
    hard_->outhi ();
    for (x = 0; x < 16; x++)
      {
        hard_->clkhi ();
        waitus (clk_delay + 5);
        if (x > 7)
	  if (hard_->inbit ())
	    ind |= (1 << x);
        hard_->clklo ();
        waitus (clk_delay + 3);
      }
    return ind & wmask ();
}

unsigned int pic18F::get_byte ()
{
    unsigned int x, ind = 0;

    // Normal (safe) state...
    hard_->rwread ();

    send_cmd (2);
    hard_->outhi ();
    for (x = 0; x < 16; x++)
      {
        hard_->clkhi ();
        waitus (clk_delay + 5);
        if (x > 7)
	  if (hard_->inbit ())
	    ind |= (1 << (x - 8));
        hard_->clklo ();
        waitus (clk_delay + 3);
      }
    return ind;
}

/* ------------------------------------------------------------
      PIC 18 family uses bytes adresses
   ------------------------------------------------------------ */
bool pic18F::byteAdresses () const
{
    return true;
}

/* ------------------------------------------------------------
      PIC 18 family has 16 bits words
   ------------------------------------------------------------ */
int pic18F::wmask ()
{
    return 0xFFFF;
}

/* ------------------------------------------------------------
      PIC 18 family uses 4 bits commands
   ------------------------------------------------------------ */
void pic18F::send_cmd (unsigned int d)
{
    send_bits (d, 4);
}

void pic18F::send_bits (unsigned int d, int nbb)
{
    int x;

    //Keep the write state...
    hard_->rwwrite ();

    for (x = nbb; x; --x)
      {
        hard_->clkhi();	// Clock high

        if (d & 0x01)
	  hard_->outhi (); // Output to 1
        else
	  hard_->outlo (); // Output to 0

        waitus (clk_delay + 5);
        hard_->clklo (); // Clock lo
        waitus (clk_delay + 3);
        d >>= 1; // Move the data over 1 bit
      }
    hard_->outhi ();

    // Normal (safe) state...
    hard_->rwread ();
}
