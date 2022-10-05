/*************************************************************************** 
   parport is a wrapper class for an easy access
   to the parallel port on PC machines.
   This class is NOT PIC programmer dependant

   Code written by Alain Gibaud, 
   (inspired by  Brian C. Lane's one, but mostly rewritten).
 ***************************************************************************/

#include <cerrno>
#include <cstdio>
#include <iostream>
#include <unistd.h>
using namespace std;

#include "uparport.hh"

/* ----------------------------------------------------------------------- 
   This table allows to control each pin of the port

   Rrequest : code of the ioctl request for reading this pin
   Wrequest : code of the ioctl request for writing this pin
   mask : mask to isolate the corresponding bit in the relevant port
   img : cache byte for the relevant port
         (data port, status port, control port)
   ----------------------------------------------------------------------- */
#define NA 0
struct parport::pintable parport::pin_table[26] =
  {
    {NA, NA, 0x00, &parport::errimg}, /* pin 0	Invalid pin # */
    {PPRCONTROL, PPWCONTROL, 0x01, &parport::controlimg}, /* pin 1 (out) !strobe */
    {PPRDATA, PPWDATA, 0x01, &parport::dataimg}, /* pin 2 (out)	Data 0 */
    {PPRDATA, PPWDATA, 0x02, &parport::dataimg}, /* pin 3 (out)	Data 1 */
    {PPRDATA, PPWDATA, 0x04, &parport::dataimg}, /* pin 4 (out)	Data 2 */
    {PPRDATA, PPWDATA, 0x08, &parport::dataimg}, /* pin 5 (out)	Data 3 */
    {PPRDATA, PPWDATA, 0x10, &parport::dataimg}, /* pin 6 (out)	Data 4 */
    {PPRDATA, PPWDATA, 0x20, &parport::dataimg}, /* pin 7 (out)	Data 5 */
    {PPRDATA, PPWDATA, 0x40, &parport::dataimg}, /* pin 8 (out)	Data 6 */
    {PPRDATA, PPWDATA, 0x80, &parport::dataimg}, /* pin 9 (out)	Data 7 */
    {PPRSTATUS, NA, 0x40, &parport::statusimg}, /* pin 10 (in) !ack */
    {PPRSTATUS, NA, 0x80, &parport::statusimg},	/* pin 11 (in) busy */
    {PPRSTATUS, NA, 0x20, &parport::statusimg},	/* pin 12 (in) Pout */
    {PPRSTATUS, NA, 0x10, &parport::statusimg},	/* pin 13 (in) Select */
    {PPRCONTROL, PPWCONTROL, 0x02, &parport::controlimg}, /* pin 14 (out) !feed */
    {PPRSTATUS, NA, 0x08, &parport::statusimg}, /* pin 15 (in) !error */
    {PPRCONTROL, PPWCONTROL, 0x04, &parport::controlimg}, /* pin 16 (out) !init */
    {PPRCONTROL, PPWCONTROL, 0x08, &parport::controlimg}, /* pin 17 (out) !SI */
    {NA, NA, 0x00, &parport::errimg}, /* pin 18 GND */
    {NA, NA, 0x00, &parport::errimg}, /* pin 19 GND */
    {NA, NA, 0x00, &parport::errimg}, /* pin 20 GND */
    {NA, NA, 0x00, &parport::errimg}, /* pin 21 GND */
    {NA, NA, 0x00, &parport::errimg}, /* pin 22 GND */
    {NA, NA, 0x00, &parport::errimg}, /* pin 23 GND */
    {NA, NA, 0x00, &parport::errimg}, /* pin 24 GND */
    {NA, NA, 0x00, &parport::errimg}, /* pin 25 GND */
  };

parport::parport ()
{
    controlimg = dataimg = statusimg = 0;
    fd = -1;
}

parport::~parport ()
{
    close ();
}

int parport::yield ()
{
    if (fd == -1)
      return -1;
    return ioctl (fd, PPYIELD);
}

/* ----------------------------------------------------------------------- 
   Open a port which name is /dev/parport[0-9]
   (or /dev/parports/[0-9] for devfs systems)

   Returns:
   with fd == -1 if there is any problem
   
   Note:
   A common problem is that /dev/parportx is not RW enabled
   ----------------------------------------------------------------------- */
void parport::setport (const char *np)
{
    if (fd != -1)
      return; // already open

    int k;

    if (((sscanf (np, "/dev/parport%d", &k) == 1) || (sscanf (np, "/dev/parports/%d", &k) == 1))
	&& (k >= 0) && (k < 10))
      {
        // seems correct ..
        fd = open (np, O_RDWR);

        if (fd != -1)
	  {
            if ((k = ioctl (fd, PPCLAIM)) != 0)
	      {
                char b[200];
                sprintf (b, "parport::setport (%.150s must be rw enabled)", np);
                perror (b);
                ::close (fd);
                fd = -1;
	      }
	  }
        else
	  perror ("Cannot open parallel port");
      }
    else
      cerr << "Bad port specification " << np << endl;
}

int parport::writedata (unsigned char x)
{
    int s = ioctl (fd, PPWDATA, &x);
    if (s)
      perror ("parport::writedata");
    else
      dataimg = x;
    return s;
}

int parport::readdata (unsigned char& x)
{
    int s = ioctl (fd, PPRDATA, &x);
    if (s)
      perror ("parport::readdata");
    else
      dataimg = x;
    return s;
}

int parport::writecontrol (unsigned char x)
{
    int s = ioctl (fd, PPWCONTROL, &x);
    if (s)
      perror ("parport::writecontrol");
    else
      controlimg = x;
    return s;
}

int parport::readcontrol (unsigned char& x)
{
    int s = ioctl (fd, PPRCONTROL, &x);
    if (s)
      perror ("parport::readcontrol");
    else
      controlimg = x;
    return s;
}

int parport::writestatus (unsigned char x)
{
    int s = ioctl (fd, NA, &x);
    if (s)
      perror ("parport::writestatus");
    else
      statusimg = x;
    return s;
}

int parport::readstatus (unsigned char& x)
{
    int s = ioctl (fd, PPRSTATUS, &x);
    if (s)
      perror ("parport::readstatus");
    else
      statusimg = x;
    return s;
}

/* -----------------------------------------------------------------------
   Pin set : if pin < 0 
   => polarity reversed and actual pin number is abs (pin)
   -----------------------------------------------------------------------*/ 
// turn on an output
void parport::biton (int pin)
{
    int apin = pin > 0 ? pin : -pin;
    if ((writerequest (apin) == NA) || ! isok ())
      return; // security

    unsigned char c = (pin > 0) ? reg (apin) | mask (apin) : reg (apin) & ~mask (apin);

    if (ioctl (fd, writerequest (apin), &c) == 0)
      reg (apin) = c;
    else
      perror ("parport::biton");
}

// turn off an output
void parport::bitoff (int pin)
{
    int apin = pin > 0 ? pin : -pin;
    if ((writerequest (apin) == NA) || ! isok ())
      return; // security

    unsigned char c = (pin > 0) ? reg (apin) & ~mask (apin) : reg (apin) | mask (apin);

    if (ioctl (fd, writerequest (apin), &c) == 0)
      reg (apin) = c;
    else
      perror ("parport::bitoff");
}

// read a pin -assuming this is an input one !-
int parport::readbit (int pin)
{
    int apin = pin > 0 ? pin : -pin;
    if ((readrequest (apin) == NA) || ! isok ())
      return 0;

    unsigned char c = 0;
    if (ioctl (fd, readrequest (apin), &c) == 0)
      reg (apin) = c;
    else
      perror ("parport::readbit");

    /* If the data is not inverted, return the normal state. Otherwise,
       invert it */
    return (pin > 0 ? c : ~c) & mask (apin);
}

/* -----------------------------------------------------------------------
   Returns 
   
   0 : The parallel port  pname is present in the system and is RW enabled
   -1 : The port is present but not RW enabled
   -2 : the port cannot be opened (not present ?)
   -----------------------------------------------------------------------*/ 
int parport::probePort (const char *pname)
{
    int fd = open (pname, O_RDWR);
    if (fd == -1)
      return -2;

    int k = ioctl (fd, PPCLAIM);

    if (k != 0)
      {
	::close (fd);
	return -1;
      }

    ioctl (fd, PPRELEASE);
    ::close (fd);

    return 0;
}

void parport::close ()
{
    if (fd != -1)
      {
        int stat = ioctl (fd, PPRELEASE);
        ::close (fd);
        if (stat != 0)
	  perror ("parport::~parport");
      }
    fd = -1;
}

bool parport::isok ()
{
    return fd != -1;
}

const char *parport::inputPinsList ()
{
    return "10 (/ACK),11 (BUSY),12 (PAPER),13 (SELin),15 (/ERROR)";
}

const char *parport::outputPinsList ()
{
    return "1 (/DS),2 (D0),3 (D1),4 (D2),5 (D3),6 (D4),7 (D5),8 (D6),9 (D7),14 (LF),16 (PRIME),17 (SELout),25 (NA)";
}
