/***************************************************************************
                          port.hh  -  description
                             -------------------
    begin                : Thu Dec 18 2003
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
/* Abstract class (interface) specifying an hardware port.
   @author Alain Gibaud */

#ifndef PORT_HH
#define PORT_HH


class port
{
public:
    port ();
    virtual ~port ();
    /* Try to open the device
       The state of the port after opening can be tested by isok ()
       An already opened port is not reopen, and no error is reported */
    virtual void setport (const char *device) = 0;
    // returns the state of pin (1 if H)
    virtual int readbit (int pin) = 0;
    // true if the port is properly open
    virtual bool isok () = 0;
    // set the pin L
    virtual void bitoff (int pin) = 0;
    // Set the pin H
    virtual void biton (int pin) = 0;
    // Closes the port and mark it as closed
    virtual void close () = 0;
    // See the outputPinsList method
    virtual const char *inputPinsList () = 0;
    /* Returns a comma separated output pin list for this port
       Each element of the list must be a pin number and may be followed
       by a pin name.
       Ex.: 1 (/STROBE), 2 (D0), 3 (D1), 4 (D2) ... */
    virtual const char *outputPinsList () = 0;
};

#endif
