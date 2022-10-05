/***************************************************************************
                          pic18F.hh  -  description
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
/* This is the (abstract) base class for pic 18XXX devices.
   Its main purpose is the factorization of pic18F common code
   @author Alain Gibaud */

#ifndef pic18F_HH
#define pic18F_HH

#include "pic.hh"


class pic18F : public pic
{
public:
    pic18F ();
    ~pic18F ();

    // Reimplemented because 18xxx devices use byte adresses instead of word addresses
    virtual bool byteAdresses () const;
    // 18xxx devices use 16 bits words
    virtual int wmask ();
    /* Send a word of data to the PIC

       Parameters:
       d: The word to be sent to the PIC. */
   virtual void send_data (unsigned int d);
    /* Read a word from the PIC through the data line
       
       Return:
       the word */
   virtual unsigned int get_word ();
    /* Read a byte from the PIC through the data line
       
       Return:
       this byte */
   virtual unsigned int get_byte ();
    /* Send a command to the PIC through the data line
       
       Parameters:
       d: The command to be sent to the PIC. */
   virtual void send_cmd (unsigned int d);

   virtual void send_bits (unsigned int d, int nbb);  
};

#endif
