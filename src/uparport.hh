/*************************************************************************** 
 parport is a wrapper class for an easy access
 to the parallel port on PC machines.
   
 Notes:  
 - User space version: This code does not need to be executed with root
   privileges.
 - Everything in this class is NOT PIC programmer dependant.
   
 Copyright 2002 by Alain Gibaud
 ***************************************************************************/

#ifndef UPARPORT_HH
#define   UPARPORT_HH

extern "C"
{
#include <sys/ioctl.h> 
// in RedHat & Mandrake distro, ppdev.h is into /usr/include/linux 
#include <linux/ppdev.h>   
#include <sys/time.h>
// #include <sys/io.h> 
#include <fcntl.h>
}           

#include "port.hh"

/*  IO ports to manage lp(s)

This comments come from Brian's code ..

  Low Level code for Linux PIC programmer
  Copyright 1994-1998 by Brian C. Lane

   Base + 0   Data I/O Address    TTL Output Only/(Tristate->Input)
   ---------------------------------------------------------------------
   Bit:  7       6       5       4       3       2       1       0
   Pin:  9       8       7       6       5       4       3       2
   ---------------------------------------------------------------------

   Base + 1   Status I/O Address  TTL Input Only
   ---------------------------------------------------------------------
   Bit:  7 Inv   6       5       4       3       2       1       0
   Pin:  11      10      12      13      15      NC      NC      NC 
   ---------------------------------------------------------------------

   Base + 2   Control I/O Address Open-Collector Output/(Input:WR=>04h)
   ---------------------------------------------------------------------
   Bit:  7       6       5       4       3 Inv   2       1 Inv   0 Inv
   Pin:  NC      NC  NC/(BiDir) Int      17      16      14      1 
   ---------------------------------------------------------------------

   Copyright to Public Domain by Richard Steven Walz (C) 1996
   ftp://ftp.armory.com/pub/user/rstevew/LPT/simplpt.faq

   This defines the pin # to ioport mappings. offset is relative to
   0x378 and mask is the pins bit position in the byte
 */
 /* Note: Base adresses are (see linux IO port programming HOWTO)
    0x3BC for /dev/lp0
    0x378 for /dev/lp1
    0x278 for /dev/lp2
    These adresses are not used anymore with the new parport driver 
 */


class parport : public port
{
private:
    int fd;
    static struct pintable
      {
        unsigned int Rrequest, Wrequest;
        unsigned char mask;
        // memory image of the port, needed for bit level access
        unsigned char parport::*img;
      } pin_table[26];
       
    // current image of the ports 
    unsigned char dataimg, statusimg, controlimg, errimg; 
    
    // inline methods for accessing each bit
    unsigned char mask (int pin) { return pin_table[pin].mask; } 
    int writerequest (int pin) { return pin_table[pin].Wrequest; } 
    int readrequest (int pin) { return pin_table[pin].Rrequest; } 
    unsigned char& reg (int pin) { return this->*(pin_table[pin].img); } 
    int yield (); // temporary release the port for multi access

public:
    parport ();
    virtual ~parport ();
    virtual bool isok (); // true if port has been successfully opened
    // select port by name /dev/lp0 == /dev/parport0
    virtual void setport (const char *); 
    // turns a pin on/off with polarity depending on the sign of pin
    virtual void biton (int pin); // turn a pin low, assuming this is an output one
    virtual void bitoff (int pin); // turn a pin low, assuming this is an output one
    virtual int readbit (int pin); // read a pin, assuming this is an input one
    
    // register-oriented (rather than pin oriented) I/O
    int writedata (unsigned char x);
    int readdata (unsigned char& x);
    int writestatus (unsigned char x);
    int readstatus (unsigned char& x);
    int writecontrol (unsigned char x);
    int readcontrol (unsigned char& x);

  
    /* ----------------------------------------------------------------------- 
       Returns:

       0 : The parallel port pname is present in the system and is RW enabled
       -1 : The port is present but not RW enabled
       -2 : the port cannot be opened (not present ?)
       ----------------------------------------------------------------------- */
    static int probePort (const char *pname);

    virtual void close ();
    /* ----------------------------------------------------------------------- 
       Returns:

       A string describing each output pin (pin number and pin name)
       ----------------------------------------------------------------------- */
    virtual const char *outputPinsList ();

    /* ----------------------------------------------------------------------- 
       Returns:

       A string describing each input pin (pin number and pin name)
       ----------------------------------------------------------------------- */
    virtual const char *inputPinsList ();
};

#endif
