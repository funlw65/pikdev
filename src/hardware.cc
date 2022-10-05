/* This class modelize the programmer hardware
   (typically, connection between DB25 and PIC pins)
   Code by Alain Gibaud, from Brian C. Lane's one */

#include <cstring>
#include <iostream>
using namespace std;

#include "hardware.hh"
#include "serport.hh"
#include "uparport.hh"


hardware::hardware() : CLK_delay (0), ok (false), the_port (0)
{
}

hardware::~hardware ()
{
    if (the_port)
      delete the_port;
    the_port = 0;
    ok = false;
}

// polarity of the pin is normal is pin number > 0
// reversed if < 0
void hardware::setpins (const char *port_type, const char *portname,
			int vpp, int vdd, int clk, int datao, int datai,
			int rw, int clkdelay)
{
    // close and deallocate if needed
    if (the_port)
      delete the_port;
    // actual port creation
    if (strcmp (port_type, "parallel") == 0)
      the_port = new parport;
    else if (strcmp (port_type, "serial") == 0)
      the_port = new serport;
    else
      assert ("Unknown port type in hardware::setpins(..), send report to author." == 0);

    // port init
    the_port->setport (portname);
    if (! the_port->isok ())
      {
	delete the_port;
	the_port = 0;
	return;
      }

    VPP_pin = vpp;
    VDD_pin = vdd;
    CLK_pin = clk;
    DATAO_pin = datao;
    DATAI_pin = datai;
    DATARW_pin = rw;
    CLK_delay = clkdelay;
    // Keep the safe state of rw (read)...
    outlo (); clklo ();
    vppoff (); vddoff (); rwread ();
    ok = true;
}

// this is my specific hardware test
int hardware::hardware_test ()
{
    char t[80];

    cout << endl << "Please execute this hardware test without any PIC"
                    " connected to your programmer" << endl << endl;

    do
      {
	cout << "Ready for tests ? (y/n) ";
	cin >> t;
      }
    while (t[0] != 'y' && t[0] != 'n');
    if (t[0] == 'n')
      return -1;

    // stage 1 - hard initialization
    // setpins("parallel","/dev/parport0", -5, -4, 3, 2, 10) ;
    // ++Mirko!!
    // Default RW line not used...
    setpins ("serial","/dev/ttyS0", 3, 5, 7, 4, 8, 0, 0);
    // --Mirko!!

    if (! ok)
      {
	cout << "Hardware initialization error" << endl;
	return 1;
      }

    // ++Mirko!!
    // By Ciri 11/3/2004...
    // From here to the end of function i haven't modified nothing...
    // --Mirko!!

    // stage 2 - all lines off
    vppoff ();
    vddoff ();
    clklo ();
    outlo ();
    cout << "All the following lines must be 0V : " << endl
	 << "16F84 pin 13 (data out)"<< endl
	 << "16F84 pin 12 (clock)"<< endl
	 << "16F84 pin 14 (VDD=power)"<< endl
	 << "16F84 pin 4  (VPP=prog)"<< endl;
    do
      {
	cout << "OK ? (y/n) ";
	cin >> t;
      }
    while (t[0] != 'y' && t[0] != 'n');

    if (t[0] == 'n')
      return 2;

    // stage 3 - data out check
    outhi ();
    cout << "16F84 pin 13 (data out) must be 5V" << endl;
    do
      {
	cout << "OK ? (y/n) ";
	cin >> t;
      }
    while (t[0] != 'y' && t[0] != 'n');
    outlo ();

    if (t[0] == 'n')
      return 3;

    // stage 4 - clock check
    clkhi ();
    cout << "16F84 pin 12 (clock) must be 5V" << endl;
    do
      {
	cout << "OK ? (y/n) ";
	cin >> t;
      }
    while (t[0] != 'y' && t[0] != 'n');
    clklo ();

    if (t[0] == 'n')
      return 4;

    // stage 5 - VDD check
    vddon ();
    cout << "16F84 pin 14 (power) must be 5V" << endl;
    do
      {
	cout << "OK ? (y/n) ";
	cin >> t;
      }
    while (t[0] != 'y' && t[0] != 'n');
    vddoff ();

    if (t[0] == 'n')
      return 5;

    // stage 6 - VPP check
    vppon ();
    cout << "16F84 pin 4 (VDD) must be between 13V and 14V" << endl;
    do
      {
	cout << "OK ? (y/n) ";
	cin >> t;
      }
    while (t[0] != 'y' && t[0] != 'n');
    vppoff ();

    if (t[0] == 'n')
      return 6;

    // stage 7 - test input data
    // set data out hi, because bi-directionnal
    // on pin 13 uses the open collector capability of 7407
    outhi ();
    int in = inbit ();
    if (! in)
      {
	cout << "DataIn error (16F84 pin 13) : must be 5V and is not" << endl;
	return 7;
      }
    cout << "Please set 16F84 pin 13 (DataIn) low "
	 << "(connect it to 16F84 pin 5 - GND)" << endl;
    do
      {
	cout << "Done ? (y/n) ";
	cin >> t;
      }
    while (t[0] != 'y' && t[0] != 'n');

    in = inbit ();
    if (in)
      {
	cout << "DataIn error (pin 13) : must be 0V and is not" << endl;
	return 7;
      }

    cout << "Congratulations! - Hardware is OK." << endl;

    return 0;
}

// this is my specific speed test
int hardware::timing_test ()
{
    char t[80];
    int cmd;
    long loop = 50000000;

    cout << endl << "Please execute this hardware test without any PIC"
                    " connected to your programmer" << endl << endl;

    do
      {
	cout << "Ready for tests ? (y/n) ";
	cin >> t;
      }
    while (t[0] != 'y' && t[0] != 'n');
    if (t[0] == 'n')
      return -1;

    // stage 1 - hard initialization
    // Default Line RW not used...
    setpins ("parallel", "/dev/parport0", -5, -4, 3, 2, 10, 25, 0);

    if (! ok)
      {
	cout << "Hardware initialization error" << endl;
	return 1;
      }

    // By Ciri 11/3/2004...
    // From here to the end of function i have modified nothing...

    // stage 2 - all lines off
    vppoff ();
    vddoff ();
    clklo ();
    outlo ();

    // stage 3 - choose test
    cout << "Remember : " << endl
	 << "16F84 pin 5  is GND" << endl
	 << "16F84 pin 13 is data-out" << endl
	 << "16F84 pin 12 is clock" << endl;

    cout << loop << " periods test .... " << endl;

    cout << "1 - Maximum speed clock " << endl
	 << "2 - 100us half period "<< endl
	 << "3 - load data 0x2AAA (programmer->chip)" << endl
	 << "4 - end" << endl;
    do
      {
	cout << "CMD (1-4)>> ";
	cin >> cmd;
      }
    while (cmd < 1 || cmd > 4);

    if (cmd == 4)
      return 2;
    else if (cmd == 1)
      {
	for (long j = 0; j < loop; ++j)
	  {
	    clklo ();
	    clkhi ();
	  }
      }
    else if (cmd == 2)
      {
	for (long j = 0; j < loop; ++j)
	  {
	    clklo (); waitus (100);
	    clkhi (); waitus (100);
	  }
      }
    else if (cmd == 3)
      {
	for (long j = 0; j < loop; ++j)
	{
	  int d = 0x2AAA;
	  int x;

	  d &= 0x3FFF; // mask unused msb
	  d <<= 1; // insert start bit

	  for (x = 0; x < 16; x++)
	  {
	    clkhi ();
	    if (d & 0x01)
	      outhi ();
	    else
	      outlo ();
	    clklo ();
	    d >>= 1;
	  }
	  outhi ();
	}
      }

    return 0;
}

// Wait us Microseconds (from Brian C Lane's code)
void hardware::waitus (unsigned int us)
{
    struct timeval tv, wt;

    gettimeofday (&tv, NULL);

    wt.tv_usec = (tv.tv_usec + us) % 1000000;
    wt.tv_sec = tv.tv_sec + ((tv.tv_usec + us) / 1000000);

    // loop until time is up
    for (;;)
      {
	gettimeofday (&tv, NULL);
	if (tv.tv_sec > wt.tv_sec)
	  return;
	if (tv.tv_sec == wt.tv_sec)
	  {
	    if (tv.tv_usec > wt.tv_usec)
	      return;
	  }
      }
}

// Returns the port  currently used by programmer hardware
port *hardware::getPort ()
{
    return the_port;
}

// Close and deallocated the current port
void hardware::releasePort ()
{
    if (the_port)
      delete the_port;
    the_port = 0;
    ok = false;
}
