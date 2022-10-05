/***************************************************************************
    copyright            : Alain Gibaud
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
/* ----------------------------------------------------------------------
   "Universal" ;-) pic programmer for parallel/serial port
   ---------------------------------------------------------------------- */

#ifndef PKP_VERSION
#define PKP_VERSION 1.4.0
#endif

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "cmdParser.hh"
#include "PicFactory.hh" 
#include "progressListener.hh"


class prog : public progressListener
{
protected:
    hardware *hard_;
    pic *cpic; // current pic

    // hardware pinout (used for configuration only)
    int vpp, vdd, clock, datao, datai, rw, delay;
    char port[256]; // linux port device (i.e.: /dev/parportXX)
    char port_type[256]; // linux port device (i.e.: /dev/parportXX)
    string oldfile, oldpic;
    bool verbose; // normal mode : true, machine friendly mode = false 

    void usage ();
    pic *choosePic (const string& picname);

    void displayFlavors ();
    int read_config ();
    void saveDefaults ();
    void loadDefaults ();
    void showDefaults ();
    void error (const string& txt, int code = 200, const string& other = "");

public:
    prog ();
    virtual ~prog ();

    // command interpreter for AN mode
    int processCommands ();
    int processCommands (int argc, char *argv[]);
    bool mfmode () { return ! verbose; }
    virtual void updateProgressBar (int percentage);
    virtual void updateProgressText (const char *);
};

int prog::read_config ()
{
    ifstream fp;
    char path[1024];
    int k, line;

    /* Defaults pin number (negative value means negative logic) :
       these initializations conforms to David Tait's hardware */
    vpp = -5;
    vdd = -4;
    clock = 3;
    datao = 2;
    datai = 10;
    rw = 25;
    delay = 0;

    // default port
    strcpy (port, "/dev/parport0"); // 0x378


    // Make a path to the configuration file
    // try to open a local .picprgrc
    fp.open (".pkprc");
    if (! fp.is_open ())
      {
        // try $(HOME)/.pkprcqrc
        strcpy (path, getenv ("HOME"));
        if (*path == 0)
	  return 1; // no HOME symbol ...
        else
	  strcat (path, "/.pkprc");
        fp.open (path);
        if (! fp.is_open ())
	  return 1;
      }

    *port_type = 0;

    // Read the file 1 line at a time
    for (line = 1; fp.getline (path, 1023); ++line)
      {
        // Ignore comments and blank lines
        if (path[0] == '#' || path[0] == '\n')
	  continue;
        // read port type (ie : "serial" or "parallel")
        else if (sscanf (path, "type=%s", port_type) != 0);
        // Set the /dev/... port to use
        else if (sscanf (path, "port=%s", port) != 0);
        // Set the clock delay
        else if (sscanf (path, "delay=%d", &delay) != 0);        
        // Set the VPP pin number and polarity
        else if (sscanf (path, "vpp=%d", &k) == 1 && k > -26 && k < 26 && k != 0)
	  vpp = k;
        // Set the Vdd pin number and polarity
        else if (sscanf (path, "vdd=%d", &k) == 1 && k > -26 && k < 26 && k != 0)
	  vdd = k;
        // Set the Clock pin number and polarity
        else if (sscanf (path, "clock=%d", &k) == 1 && k > -26 && k < 26 && k != 0)
	  clock = k;
        // Set the Data to PIC pin number and polarity
        else if (sscanf (path, "datao=%d", &k) == 1 && k > -26 && k < 26 && k != 0)
	  datao = k;
        // Set the Data from PIC pin number and polarity
        else if (sscanf (path, "datai=%d", &k) == 1 && k > -26 && k < 26 && k != 0)
	  datai = k;
        // Set the Data direction pin number and polarity
        else if (sscanf (path, "rw=%d", &k) == 1 && k > -26 && k < 26 && k != 0)
	  rw = k;
        // else, keyword error, abort
        else
	  {
            fp.close ();
            cerr << "Unknown keyword : [" << path << "], line " << line << endl;
            return 2;
	  }
      }
    fp.close ();

    // hack ..
    if (*port_type == 0)
      return 1;
    hard_ = new hardware;
    // data configuration is read, configure hardware device now
    hard_->setpins (port_type, port, vpp, vdd, clock, datao, datai, rw, delay);
    if (! hard_->isok ())
      {
        delete hard_;
	hard_ = 0;
        return 3;
      }
    return 0;
}

prog::prog ()
{
    hard_ = 0;
    cpic = 0;
    oldfile = oldpic = "";
    verbose = true;

    int r = read_config ();

    if (r == 1)
      {
        cerr << "--------------------------------------------------------------------------------- "
	     << endl;
        cerr << "You do not have any config file, or maybe the structure of this file \n"
	        "has changed since previous version and some informations are missing."
	     << endl << endl;
        ofstream f (".pkprc");
        if (f.is_open ())
	  {
            cerr << "A new .pkprc file has been created (with default values) in the current directory "
		 << endl;
            cerr << "Please edit this file to fit your hardware configuration" << endl;
            cerr << "and run pkp again." << endl;
            cerr << "--------------------------------------------------------------------------------- "
		 << endl;
            f << "# settings for parallel port device : \n"
	         "# (use /dev/parports/x if your system is devfs based)\n"
	         "port=/dev/parport0\n"
	         "type=parallel\n"
	         "# For serial port programmers, uncomment the following 2 lines\n"
		 "# and modify the pin assignation\n"
		 "#port=/dev/ttyS0\n#type=serial\n"
		 "# pins assignation : negative value means inverted signal\n"
		 "# the following values are correct for classic Tait parallel programmer\n"
		 "vpp=-5\nvdd=-4\nclock=3\ndatao=2\ndatai=10\nrw=25\ndelay=0\n"
		 "# the following values are correct for classic JDM serial programmer\n"
		 "#vpp=3\n#vdd=5\n#clock=7\n#datao=4\n#datai=8\n#rw=5\n#delay=0\n";
            f.close ();
	  }
        exit (-1);
      }
    else if (r == 2)
      error ("Keyword error in .pkprc config file");
    else if (r == 3)
      error ("Hardware initialization error");
}

prog::~prog ()
{
    if (hard_ != 0)
      delete hard_;
}

void prog::displayFlavors ()
{
    if (verbose)
      cout << "Available PIC devices :" << endl;
    string names (PicFactory::factory ()->getPicNames ()), name;
    istringstream is (names);
    vector<string> vnames;
    while (is >> name)
      vnames.push_back (name);
    sort (vnames.begin (), vnames.end ());
    
    for (vector<string>::iterator i = vnames.begin (); i != vnames.end (); ++i)
      cout << *i << endl;
}

pic *prog::choosePic (const string& picname)
{
    pic *p;
    p = PicFactory::factory ()->getPic (picname.c_str ());
    if (p)
      {
    	(cpic = p)->setPort (hard_);
	p->setProgressListener (this);
      }
    return p;
}

#define QUOTE1(y) #y
#define QUOTE(x) (QUOTE1(x))

void prog::loadDefaults ()
{
    ifstream restore;

    oldpic = "p16f84";
    oldfile = "";
    restore.open (".pkpdefaults");
    if (restore.is_open ())
      {
	if (restore >> oldfile >> oldpic)
	  choosePic (oldpic.c_str ());
      }
}

// command line (non-interactive) mode
int prog::processCommands (int argc, char *argv[])
{
    string device, infile, str;
    int status;
    const char *msg;

    loadDefaults ();

    cmdParser com;
    // configure valid options
    com.addOption ("help");
    com.addOption ("erase");
    com.addOption ("list");
    com.addOption ("mfmode");
    com.addOption ("dump");
    com.addOption ("defaults?");
    com.addOption ("device", cmdParser::HasParameter | cmdParser::HasNoParameter);
    com.addOption ("probe");
    com.addOption ("program", cmdParser::HasParameter | cmdParser::HasNoParameter);
    com.addOption ("read", cmdParser::HasParameter | cmdParser::HasNoParameter);
    com.addOption ("verify", cmdParser::HasParameter | cmdParser::HasNoParameter);

    str = com.analyse (argc, argv);
    if (str != "")
      error (str);
    if (com.hasOption ("help"))
      {
	usage ();
	return 0;
      }
    if (com.hasOption ("mfmode"))
      verbose = false;
    if (com.hasOption ("defaults?"))
      showDefaults ();
    if (com.hasOption ("device"))
      {
	device = com.optValue ("device");
	if (choosePic (device.c_str ()) == 0)
	  error ("No such device : ", 205, device);
	else
	  oldpic = device;
      }	
    if (com.hasOption ("probe"))
      {
	if (com.hasOption ("device"))
	  {
	    device = com.optValue ("device");
	    if (choosePic (device.c_str ()) == 0)
	      error ("No such device", 205, device);
	    else
	      oldpic = device;
	  }
	string detected = cpic->read_device_name ();
	string revision = cpic->read_device_revision ();
	// TODO 
	cout << "probed device: [" << detected << "] [" << revision << "]" << endl;
	return 0;
      }
    if (com.hasOption ("list"))
      displayFlavors ();
    if (com.hasOption ("erase"))
      {            
	if (verbose && ! com.hasOption ("device"))
	  cout << "Using default device " << oldpic << endl;		
	status = cpic->erase_pic ();
	if (status)
	  error ("Erase", 201, cpic->error_msg (status));
      }
    if (com.hasOption ("program"))
      {
	infile = com.optValue ("program");
	if (infile == "")
	  {
	    if (oldfile == "")
	      error ("No file specified", 202);
	    else
	      {
		infile = oldfile;
		if (verbose)
		  cout << "Using default file " << oldfile << endl;
	      }
	  }
	msg = cpic->loadHexFile (infile.c_str ());
	if (msg != 0)
	  error ("Loading file error", 203, msg);
	oldfile = infile;
	if (verbose && ! com.hasOption ("device"))
	  cout << "Using default device " << oldpic << endl;

	status = cpic->program_pic ();
	if (status)
	  error ("Program", status, cpic->error_msg (status));
      }
    if (com.hasOption ("read"))
      {
	infile = com.optValue ("read");
	if (infile == "")
	  {
	    if (oldfile == "")
	      error ("No file specified", 202);
	    else
	      {
		infile = oldfile;
		if (verbose)
		  cout << "Using default file " << oldfile << endl;
	      }
	  }	
	oldfile = infile;

	if (verbose && ! com.hasOption ("device"))
	  cout << "Using default device " << oldpic << endl;	

	cpic->read_pic ();
	msg = cpic->writeHexFile (infile.c_str (), "inhx32");
	if (msg != 0)
	  error("File write error: ", 204, msg);
      }
    if (com.hasOption ("verify"))
      {
	infile = com.optValue ("verify");
	if (infile == "")
	  {
	    if (oldfile == "")
	      error ("No file specified", 202);
	    else
	      {
		infile = oldfile;
		if (verbose)
		  cout << "Using default file " << oldfile << endl;
	      }
	  }
	msg = cpic->loadHexFile (infile.c_str ());
	if (msg != 0)
	  error ("Loading file error: ", 203, msg);
	oldfile = infile;
	if (verbose && ! com.hasOption ("device"))
	  cout << "Using default device " << oldpic << endl;

	int status = cpic->verify_pic (cout);
	if (status != 0)
	  error ("File write error", status, cpic->error_msg (status));
      }
    if (com.hasOption ("dump"))
      {
	int blk = cpic->display_mem (cout);
	if (verbose)
	  cout << endl << "(" << blk << " block(s) detected)" << endl;
      }	

    saveDefaults ();
    return 0;
}

void prog::usage ()
{	
    cout << "Usage:  pkp options" << endl;

    cout << " options:" << endl;
    cout << " --help : display this help and exit" << endl;
    cout << " --mfmode : machine-friendly mode. Only emit mandatory messages" << endl;
    cout << " --defaults? : show default device and default file" << endl;
    cout << " --device[=<device name>] : select a target device" << endl;
    cout << " --probe : try to detect target device and exit" << endl;
    cout << " --list : list available devices" << endl;
    cout << " --erase : erase device" << endl;
    cout << " --program=[<file name>] : program+verify device from specified hex file" << endl;
    cout << " --read=[<file name>] : read device then save data to hex file" << endl;
    cout << " --verify=<file name> : compare device memory to hex file" << endl;
    cout << " --dump : dump programmer internal memory (to be used with read,program or verify)"
	 << endl;
    cout << endl << "a single command may include several options in any order" << endl
	 << "options will be executed in the order of this help text." << endl
	 << "Specified device/filename becomes the defaults for the next command invocation "
	 << endl
	 << "so pkp can often be used without specifying any device nor filename." << endl;
}

int prog::processCommands ()
{
    int i, blk;
    char t[100];
    unsigned int value, adr;
    string newfile, comm, n1;

    ofstream sav;
    ifstream restore;
    char tt[5];

    if (cpic == 0)
      choosePic ("p16f84");

    loadDefaults ();

    cout << endl;
    cout << "   pkp interactive mode" << endl;
    cout << "   (PiKdev programming engine V" << QUOTE(PKP_VERSION) << ")" << endl;
    cout << "   Alain Gibaud <alain.gibaud@free.fr>" << endl;
    cout << "   (See http://pikdev.free.fr for details)" << endl;
    cout << "   For entering command line mode, use \"pkp --help\"" << endl;
 
    for (;;)
      {
        cout << endl << "  (current device: " << oldpic
	     << ", current file: " << oldfile << ")" << endl;
        cout << endl
	     << " (L)oad .HEX file        --   (S)ave .HEX file" << endl
	     << " (E)rase (if applicable) --   (R)ead PIC" << endl
	     << " (P)rogram+verify        --   (V)erify " << endl
	     << " (D)ebug ON              --   (d)ebug OFF" << endl
	     << " (C)hoose PIC device     --   (Q)uit" 	<< endl
	     << " (M)odify memory         --   display (B)uffer " << endl;

        cout << endl << ">> ";
        getline (cin, comm, '\n');

        tt[0] = 0;

        sscanf (comm.c_str (), "%4s", tt);

        switch (tt[0])
	  {
	    case 'L':
	    case 'l':
	      cout << "File name ? (<return> will load \"" << oldfile << "\") ";
	      getline (cin, newfile, '\n');
	      if (newfile == "")
		newfile = oldfile;
	      oldfile = newfile;
	      if (! cpic->load_hex (newfile.c_str ()))
		cout << "Loading " << newfile << " failed ... " << endl;
	      break;
	    case 'S':
	    case 's':
              cout << "File name ? ";
              cin.getline (t, 100);
              sscanf (t, "%s", t); // skip blanks
              if (cpic->write_hex (t, IHX32) == 1)
	        cout << "Writing " << t << " failed ... " << endl;
              break;
	    case 'E':
	    case 'e':
              i = cpic->erase_pic ();
              cout << endl << cpic->error_msg (i) << endl;
              break;
	    case 'P':
	    case 'p':
              i = cpic->program_pic ();
              cout << endl << cpic->error_msg (i) << endl;
              break;
	    case 'V':
	    case 'v':
              i = cpic->verify_pic ();
              cout << endl << cpic->error_msg (i) << endl;
              break;
	    case 'R':
	    case 'r':
              cpic->read_pic ();
              break;
	    case 'X':
	    case 'x':
	    case 'Q':
	    case 'q':
              saveDefaults ();
	      exit (0);
              break;
	    case 'D':
              cpic->debugon (1);
              break;
	    case 'd':
              cpic->debugoff (1);
              break;
	    case 'C':
	    case 'c':
              displayFlavors ();
              cout << endl << "PIC name ? ";
              cin.getline (t, 100); // to be able to reply with <newline>
              sscanf (t, "%s", t);
              oldpic = t;
              if (choosePic (oldpic) == 0)
	        {
                  oldpic = "p16f84";
                  cout << "No such pic, defaulted to " << oldpic << endl;
                  choosePic (oldpic);
	        }
              break;
	    case 'M':
	    case 'm':
              cout << "Memory adress ? (hex) ";
              cin.getline (t, 100);
              sscanf (t, "%x", &adr);
              cout << "Value ? (hex) ";
              cin.getline (t, 100);
              sscanf (t, "%x", &value);
              cout << hex << "(0x" << adr << ") set to "
		   << hex << "0x" << value << endl;
              cout << dec;
              cpic->set_memory (adr, value);
              blk = cpic->display_mem (cout);
              cout << endl << "(" << blk << " block(s) detected)" << endl;
              break;
	    case 'B':
	    case 'b':
              blk = cpic->display_mem (cout);
              cout << endl << "(" << blk << " block(s) detected)" << endl;
              break;
	    default:
	      cout << "             Mmmm ...  " << endl;
	      cin.clear ();
	      break;
	  }
      }

    return 0;
}

void prog::error (const string& txt, int code, const string& other) 
{
    cerr << "pkp:" << code << ":" << txt;
    if (other != "")
      cerr << " (" << other << ")";
    cerr << endl;
    exit (-1);
}

void prog::saveDefaults ()
{
    ofstream sav;
    sav.open (".pkpdefaults");
    sav << oldfile << endl << oldpic << endl;
    sav.close ();
}

void prog::showDefaults ()
{
    if (verbose)
      cout << "current device: " << oldpic << " current file: " << oldfile << endl;
    else
      cout << oldpic << " " << oldfile << endl;
}

void prog::updateProgressBar (int percentage) 
{
    if (mfmode ())
      cout << percentage << "%\n" << flush;
    else
      {
	cout << percentage << "%\r" << flush;
	if (percentage == 100)
	  cout << "\r    \r" << flush;
      }
}

void prog::updateProgressText (const char *) 
{
}

int main (int argc, char *argv[])
{
    prog p;
    int r;
    
    if (argc <= 1)
      r = p.processCommands ();
    else
      r = p.processCommands (argc, argv);

    return r;
}
