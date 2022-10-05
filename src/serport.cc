/***************************************************************************
                          serport.cc  -  description
                             -------------------
    begin                : Mon Dec 22 2003
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

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "serport.hh"
#include "hardware.hh"


serport::serport (): fd (-1)
{
}

serport::~serport ()
{
    close ();
}

/* Allocate and open the serial port
   The status of this operation can be tested with isok() */
void serport::setport (const char *device)
{
    fd = open (device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1)
      {
        char buf[300];
        sprintf (buf, "serial port %.150s must be RW enabled.", device);
        perror (buf);
        return;
      }

    tcgetattr (fd, &current_state);
    old_state = current_state;
    current_state.c_iflag = IGNBRK | IGNPAR;
    current_state.c_oflag = 0;
    current_state.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
    current_state.c_lflag = 0;
    tcsetattr (fd, TCSANOW, &current_state);

    // set up lines for "idle state"
}

bool serport::isok ()
{
    return fd != -1;
}

void serport::close ()
{
    if (fd != -1)
      {
        ioctl (fd, TIOCCBRK, 0);
        hardware::waitus (1);
        tcsetattr (fd, TCSANOW, &old_state);

        ::close (fd);
        fd = -1;
      }
}

// set the specified pin low
void serport::bitoff (int pin)
{
    if (pin < 0)
      {
        biton (-pin);
        return;
      }
    int the_bit;
    if (pin == 3) // TX
      {
        // the_bit = TIOCM_ST;
        ioctl (fd, TIOCCBRK, 0);
        return;
      }
    else if (pin == 4)  // DTR
      the_bit = TIOCM_DTR;
    else if (pin == 6) // DSR
      the_bit = TIOCM_DSR;
    else if (pin == 7) // DSR
      the_bit = TIOCM_RTS;
    else
      return; // not an output pin .
    if (ioctl (fd, TIOCMBIC, &the_bit) == -1)
      perror ("void serport::bitoff(int pin)");
}

// See bitoff (int)
void serport::biton (int pin)
{
    if (pin < 0)
      {
        bitoff (-pin);
        return;
      }
    int the_bit;
    if (pin == 3) // TX
      {
        //the_bit = TIOCM_ST;
        ioctl (fd, TIOCSBRK, 0);
// patch from serdarkoylu@yahoo.com
#define SERDAR_PATCH
#ifdef SERDAR_PATCH
	ssize_t bytes_written __attribute__ ((unused)) = write (fd, "\xFF", 1);
	usleep (2000);
#endif
        return;
      }
    else if (pin == 4) // DTR
      the_bit = TIOCM_DTR;
    else if (pin == 6) // DSR
      the_bit = TIOCM_DSR;
    else if (pin == 7) // DSR
      the_bit = TIOCM_RTS;
    else
      return; // not an output pin .
    if (ioctl (fd, TIOCMBIS, &the_bit) == -1)
      perror ("void serport::bitoff(int pin)");
}

int serport::readbit (int rpin)
{
    int bits, mask, pin = rpin < 0 ? -rpin : rpin;

    if (pin == 1)
      mask = TIOCM_CD;
    else if (pin == 2)
      mask = TIOCM_SR;
    else if (pin == 8)
      mask = TIOCM_CTS;
    else if (pin == 9)
      mask = TIOCM_RI;
    else
      mask = 0;

    if (ioctl (fd, TIOCMGET, &bits) == -1)
      {
        perror ("int serport::readbit (int pin)");
        return 0;
      }
    return (((rpin < 0) ? ~bits : bits) & mask);
}

const char *serport::inputPinsList ()
{
    return "1 (CD),2 (RX),8 (CTS),9 (RI)";
}

const char *serport::outputPinsList ()
{
    return "3 (TX),4 (DTR),6 (DSR),7 (RTS),5 (NA)";
}

/* -----------------------------------------------------------------------
   Probes the port pname
 
   Returns:
   0 = the port is present on the system and is RW enabled
   -1 = port present but not RW enabled
   -2 = not present
  ----------------------------------------------------------------------- */
int serport::probePort (const char *device)
{
    int id;
    id = open (device, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (id == -1)
      return -2;
    ::close (id);

    id = open (device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (id == -1)
      return -1;
    ::close (id);
    return 0;
}
