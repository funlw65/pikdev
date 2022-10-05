/* ------------------------------------------------------------------------
   Low Level code for Linux PIC programmer
   
   Copyright 1994-1998 by Brian C. Lane
   ---------------------------------------------------------------------
   This is the way Russ has his programmer connected
   to the parallel port.

   DB25 connections
    1 = !C0    = !PGM
    2 = D0     = CLK out
    3 = D1     = DATI (to PIC)
    11 = !S7   = DATO (from PIC)
    14 = !C1   = MODE control 0=programming  1=normal
    25 = GND

   David Tait's Parallel port PIC programmer connections:
   Might be inverted!
      
   2 = D0      = Data out
   3 = D1      = Clock out
   4 = D2      = +5v Power control (++Gib: inverted : Low = power on)
   5 = D3      = +Vpp/!MCLR control (++Gib: inverted : Low = VPP on
   10=  ACK    = Data in   
   ------------------------------------------------------------------------ */

#ifndef HARDWARE_HH
#define HARDWARE_HH

#include <cassert>
#include <iostream>
using namespace std;

#include "port.hh"

/* This class modelize the programmer hardware
   (typically, connection between DB25 and PIC pins)
   Code by Alain Gibaud, from Brian C. Lane's one */
class hardware 
{
private:
    // pinout informations
    int	VPP_pin; // Pin for VPP control
    int	VDD_pin; // Pin for Vdd control
    int	CLK_pin; // Pin for Clock control	
    int	DATAO_pin; // Pin for Data to PIC control
    int	DATAI_pin; // Pin for Data from PIC control
    int DATARW_pin; // Pin for direction of data
    int CLK_delay;
    bool ok; // true if initializations complete
    port *the_port;

public:
    hardware();
    ~hardware();
     
    // set used port and specify connections/polarity  between PIC and pins 
    // each param (except port name) is a pin number (or -pin number
    // if the polarity is reversed)
    void setpins (const char *port_type, const char *port, int vpp, int vdd, int clk,
		  int datao, int datai, int rw, int clkdelay);

    bool isok () { return ok && the_port->isok (); } 
    
    // pins manipulation methods are so simple that they can be inline
    // Input a bit on a data pin
    int inbit () { return the_port->readbit (DATAI_pin); }

    // Vpp ON
    void vppon () { the_port->biton (VPP_pin); }

    // Vpp OFF
    void  vppoff () { the_port->bitoff (VPP_pin); }

    // Vdd ON
    void vddon () { the_port->biton (VDD_pin); }

    // Vdd OFF
    void vddoff () { the_port->bitoff (VDD_pin); }

    // Clock high
    void clkhi ()
      {
        the_port->biton (CLK_pin);
        if (CLK_delay)
	  waitus (CLK_delay);
      }

    // Clock low
    void clklo ()
      {
	the_port->bitoff (CLK_pin);
	if (CLK_delay)
	  waitus (CLK_delay);
      }

    // Data high
    void outhi () { the_port->biton (DATAO_pin); }

    // Data low
    void outlo () { the_port->bitoff (DATAO_pin); }

    // RW high
    void rwwrite ()
      {
	// If valid pin execute...
	if (DATARW_pin != 0)
	  the_port->bitoff (DATARW_pin);
      }

    // Added By Ciri...
    // RW low
    void rwread ()
      {
	//If valid pin execute...
	if (DATARW_pin != 0)
	  the_port->biton (DATARW_pin);
      }
  
    // hardware test --- please use it for a newly
    // designed/builted programmer board
    // because pin assignation is hard coded in this
    // routine, you might have to edit it. ++Gib:
    int hardware_test ();

    // timing test --- please use it to ensure that
    // the program meets the  timing specifications
    int timing_test (); 

    // Wait us microseconds
    static void waitus (unsigned int us);
    // Returns the port currently used by programmer hardware
    port *getPort ();
  
public:
    int get_clock_delay () { return CLK_delay; }
    // Close and deallocated the current port
    void releasePort ();
};

#endif
