/***************************************************************************
                        genericPk2cmd.cc  -  description
                             -------------------
    begin                : Sun Feb 5 2006
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

#include <cctype>
#include <iostream>
#include <map>
#include <cstdio>
using namespace std;

#include <QByteArray>
#include <QString>

#include "Config.h"
#include "genericPk2cmd.h"


genericPk2cmd::genericPk2cmd ()
{
}

// Verify the PIC's memory against memory buffer.
int genericPk2cmd::verify_pic (ostream& out)
{
    int status = 0;

    // save current buffer to temporary file
    const char *wr = writeHexFile (".pikdev_temp_file.hex", "inhx32");
    if (wr != 0)
      {
	cerr << wr << endl;
	return 32; // TODO : implement better error handling
      }

    QString pk2;
    Config::getToolPath ("pk2cmd", pk2);
    pk2 += " -JN -P";
    pk2 += name2microchipName (name ()).c_str ();
    pk2 += " -Y -F .pikdev_temp_file.hex 2>&1";

    QByteArray bapk2 = pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");
    char line[500], msg[500], cc;
    unsigned int addr, good, bad;
    __attribute__ ((unused)) bool ok = false;
    int p100;

    if (f != 0)
      {
	progress_status = 100;
	while (fgets (line, 499, f) != 0)
	  {
	    if (sscanf (line, "Operation: %[^\n]", msg) == 1)
	      progress_status.setName (msg);
	    else if ((sscanf (line, "%d%c", &p100, &cc) == 2) && cc == '%')
	      progress_status.jump (p100);
	    else if (sscanf (line, "Verify Succeeded%c", &cc) == 1)
	      ok = true;
	    else if (sscanf (line, "Program Memory Error%c", &cc) == 1)
	      {
		status |= 1;
		out << line;
	      }
	    else if (sscanf (line, "EEData Memory Error%c", &cc) == 1)
	      {
		status |= 2;
		out << line << endl;
	      }
	    else if (sscanf (line, "Configuration Memory Error%c", &cc) == 1)
	      {
		status |= 8;
		out << line;
	      }
	    else if (sscanf (line, "ID Memory Error%c", &cc) == 1)
	      {
		status |= 4;
		out << line;
	      }
	    else if (status && (sscanf (line, "%X%X%X", &addr, &good, &bad) == 3))
	      {
		sprintf (line, "[%06X] data should be %04X but %04X found.",
			 addr, good, bad);
		out << line << endl << flush;
	      }
	  }
	pclose (f);
	progress_status.jump (100);
      }
    else
      addTextConsole ("pk2cmd cannot be executed");

    remove (".pikdev_temp_file.hex");
    return status;
}

// Program RAM buffer into the PIC
int genericPk2cmd::program_pic (ostream& out)
{
    int p100;
    unsigned int addr, good, bad;
    __attribute__ ((unused)) bool ok = false;
    remove (".pikdev_temp_file.hex");
    // save current buffer to temporary file
    const char *wr = writeHexFile (".pikdev_temp_file.hex", "inhx32");
    if (wr != 0)
      {
	out << wr << endl;
	return 32;
      }

    QString pk2;
    Config::getToolPath ("pk2cmd", pk2);
    pk2 += " -JN -P";
    pk2 += name2microchipName (name ()).c_str ();
    pk2 += " -M -F.pikdev_temp_file.hex 2>&1";

    QByteArray bapk2 = pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");
    char line[500], cc, msg[300];
    int status = 0;

    if (f != 0)
      {
	progress_status = 100;
	while (fgets (line, 499, f) != 0)
	  {
	    if (sscanf (line, "Operation: %[^\n]", msg) == 1)
	      progress_status.setName (msg);
	    else if ((sscanf (line, "%d%c", &p100, &cc) == 2) && cc == '%')
	      progress_status.jump (p100);
	    else if (sscanf (line, "Program Succeeded%c", &cc) == 1)
	      ok = true;
	    else if (sscanf (line, "Program Memory Error%c", &cc) == 1)
	      {
		status |= 1;
		out << line;
	      }
	    else if (status && sscanf (line, "%x %x %x", &addr, &good, &bad) == 3)
	      {
		sprintf (line, "[%06X] data should be %04X but %04X found.", addr, good, bad);
		out << line << endl;
	      }
	    else if (sscanf (line, "EEData Memory Error%c", &cc) == 1)
	      {
		status |= 2;
		out << line;
	      }
	    else if (sscanf (line, "Configuration Memory Error%c", &cc) == 1)
	      {
		status |= 8;
		out << line;
	      }
	    else if (sscanf (line, "ID Memory Error%c", &cc) == 1)
	      {
		status |= 4;
		out << line;
	      }
	  }
	pclose (f);
      }
    else
      addTextConsole ("pk2cmd cannot be executed");

    remove (".pikdev_temp_file.hex");
    return status;
}

// Read all device memory to internal buffer.
void genericPk2cmd::read_pic ()
{
    QString pk2;
    Config::getToolPath ("pk2cmd", pk2);
    pk2 += " -JN -P";

    pk2 += name2microchipName (name ()).c_str ();
    pk2 += " -GF.pikdev_temp_file.hex 2>&1";
    int p100;
    progress_status = 100;
    // progress_status = "Reading ..";

    QByteArray bapk2 = pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");
    char line[500], msg[500], cc;

    if (f != 0)
      {
	bool fatal_error = false;
	progress_status = 100;
	while (fgets (line, 499, f) != 0)
	  {
	    // a blinder + tard
	    if (sscanf (line, "Fatal error>%[^\n]", msg) == 1)
	      {
		clearConsole ();
		addTextConsole (QString (msg));
		fatal_error = true;
	      }
	    else if (sscanf (line, "Operation: %[^\n]", msg) == 1)
	      progress_status.setName (msg);
	    else if ((sscanf (line, "%d%c", &p100 ,&cc) == 2) && cc == '%')
	      progress_status.jump (p100);
	  }
	pclose (f);

	if (fatal_error)
	  addTextConsole ("Read PIC failed");
	else
	  {
	    const char *ld = loadHexFile (".pikdev_temp_file.hex");
	    if (ld != 0)
	      {
		clearConsole ();
		addTextConsole (QString (ld));
	      }
	  }

	// removing a non-existant file is not a problem
	remove (".pikdev_temp_file.hex");
      }
    else
      addTextConsole ("pk2cmd cannot be executed");
}

// Bulk erase device
int genericPk2cmd::erase_pic ()
{
    int status = 0;

    QString pk2;
    Config::getToolPath ("pk2cmd", pk2);

    pk2 += " -JN -P";
    pk2 += name2microchipName (name ()).c_str ();
    pk2 += " -E 2>&1";

    QByteArray bapk2 =  pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");
    char line[500], msg[500] ;

    if (f != 0)
      {
	progress_status = 100;
	while (fgets (line, 499, f) != 0)
	  {
	    if (sscanf (line, "Fatal error>%[^\n]", msg) == 1)
	      {
		clearConsole ();
		addTextConsole (QString (msg));
	      }
	  }
	pclose (f);
	progress_status.jump (100);
      }
    else
      addTextConsole ("pk2cmd cannot be executed");

    return status;
}

// Probe device. find device name and revision.
// #define MAGNUS
bool genericPk2cmd::readDevRev (string& d, string& r)
{
    bool found = false;

    QString pk2;
    Config::getToolPath ("pk2cmd", pk2);

    int famid = getPk2FamilyID (name ());
    if (famid == -1)
      return false;

    QString id;
    id = id.setNum (famid);
    pk2 += " -i -PF";
    pk2 += id;
    pk2 += " 2>&1";
    char line[500], dev[30], rev[30];
    *dev = 0;
    d = r = "";

#ifdef MAGNUS
    cout << "cmd=" << pk2.toStdString () << endl;
#endif

    QByteArray bapk2 = pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");

#ifdef MAGNUS
    cout << "f=" << (void *) f << endl;
#endif

    if (f != 0)
      {
	progress_status = 100;
	while (fgets (line, 499, f) != 0)
	  {
#ifdef MAGNUS
	    cout << " resp=" << line << endl;
#endif
	    if (sscanf (line, "Revision  = %*c%*c%s", rev) == 1)
	      r = rev;
	    else if (sscanf (line, "Device Name = PIC%s", dev) == 1)
	      {
		for (char *p = dev; *p; ++p)
		  {
		    if (isupper (*p))
		      *p = tolower (*p);
		  }
		d = "p" + string (dev);
		found = true;
	      }
	  }
	pclose (f);
	progress_status.jump (100);
      }
    else
      addTextConsole ("pk2cmd cannot be executed");

    return found;
}

/* As device and revision can be read together
   I keep revision if device is needed and
   I keed device if revision is needed
   so readDevRev is called only one time */
string genericPk2cmd::read_device_revision ()
{
    string r = revision;

    if (revision == "")
      {
	if (readDevRev (device, revision))
	  {
	    r = revision;
	    revision = "";
	  }
      }

    return r;
}

string genericPk2cmd::read_device_name ()
{
    string r = device;

    if (device == "")
      {
	if (readDevRev (device, revision))
	  {
	    r = device;
	    device = "";
	  }
      }

    return r;
}

// trivial conversion from p18fxxx to PIC18fxxx
string genericPk2cmd::name2microchipName (const string& n)
{
    return "PIC" + string (n, 1);
}

/* pk2cmd uses internal integer familly ID for family selection
   this function returns returns the family ID of a device */
int genericPk2cmd::getPk2FamilyID (__attribute__ ((unused)) const string& devName)
{
    int id = -1;
    char line[500], familyName[50], deviceName[50], altDeviceName[50];
    QString pk2;
    if (fam2ID.empty ())
      {
	Config::getToolPath ("pk2cmd", pk2);
	pk2 += " -PF 2>&1";

	QByteArray bapk2 = pk2.toLatin1 ();
	FILE *f = popen (bapk2, "r");

	if (f != 0)
	  {
	    while (fgets (line, 499, f) != 0)
	      {
		if (sscanf (line, "%d%*[ ]%[^\n]", &id, familyName) == 2)
		  fam2ID[string (familyName)] = id;
	      }
	    pclose (f);
	  }
	else
	  return -1;
      }

    // OK, we have family map - now ask for all supported chips
    Config::getToolPath ("pk2cmd", pk2);
    pk2 += " -?P 2>&1";

    id = -1;

    QByteArray bapk2 = pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");
    if (f)
      {
	for (; id == -1 && fgets (line, 300, f) != 0;)
	  {
	    // lines containing devices name always begin by PIC...
	    if (sscanf (line, "PIC%s (%[^)])%*[ ]%[^\n]", deviceName,
			altDeviceName, familyName) == 3)
	      {
		QString devname (deviceName);
		devname = "p" + devname.toLower ();
		if (devname == name ())
		  {
		    if (fam2ID.find (string (familyName)) != fam2ID.end ())
		      id = fam2ID[string (familyName)];
		  }
		else // try alternate device name
		  {
		    devname = (devicePrefix (devname.toStdString ()) + altDeviceName).c_str ();
		    if (devname == name ())
		      {
			if (fam2ID.find (string (familyName)) != fam2ID.end ())
			  id = fam2ID[string (familyName)];
		      }
		  }
	      }
	    else if (sscanf (line, "PIC%s%*[ ]%[^\n]", deviceName, familyName) == 2)
	      {
		QString devname (deviceName);
		devname = "p" + devname.toLower ();
		if (devname == name ())
		  {
		    if (fam2ID.find (string (familyName)) != fam2ID.end ())
		      id = fam2ID[string (familyName)];
		  }
	      }
	  }
	pclose (f);
      }

    return id;
}

string genericPk2cmd::devicePrefix (string dev)
{
    for (int i = dev.size () - 1; i > 0; --i)
      {
	if (! isdigit (dev[i]))
	  {
	    dev.erase (i + 1);
	    break;
	  }
      }
    return dev;
}
