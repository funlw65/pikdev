/***************************************************************************
                         genericPk2.cc  -  description
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
#include <cstdio>
#include <iostream>
using namespace std;

#include <QString>

#include "Config.h"
#include "genericPk2.h"


genericPk2::genericPk2 ()
{
}

// Verify the PIC's memory against memory buffer.
int genericPk2::verify_pic (ostream& out)
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
    Config::getToolPath ("pk2", pk2);
    pk2 += " --progress --verify .pikdev_temp_file.hex 2>&1";

    QByteArray bapk2 = pk2.toLatin1 ();

    FILE *f = popen (bapk2, "r");
    char line[500];
    unsigned int addr, osccal_err, config_err;
    int prg_err, ee_err, p100;

    if (f != 0)
      {
        progress_status = 100;
        while (fgets (line, 499, f) != 0)
	  {
            if (sscanf (line, "[%d%%]", &p100) == 1)
	      progress_status.jump (p100);
            else if (sscanf (line, "address %x", &addr) == 1)
	      out << line;
            else if (sscanf (line, "Program errors %d", &prg_err) == 1 && prg_err != 0)
	      {
                status |= 1;
                out << line;
	      }
            else if (sscanf (line, "EEPROM errors %d", &ee_err) == 1 && ee_err != 0)
	      {
                status |= 2;
                out << line;
	      }
            else if (sscanf (line, "File osccal %x", &osccal_err) == 1)
	      {
                status |= 64;
                out << line;
	      }
            else if (sscanf (line, "File config word %x", &config_err) == 1)
	      {
                status |= 8;
                out << line;
	      }
            // TODO: ID are not checked
	  }
        pclose (f);
        progress_status.jump (100);
      }
    remove (".pikdev_temp_file.hex");
    return status;
}

// Program RAM buffer into the PIC
int genericPk2::program_pic (ostream& out)
{
    int p100;
    remove (".pikdev_temp_file.hex");
    // save current buffer to temporary file
    const char *wr = writeHexFile (".pikdev_temp_file.hex", "inhx32");
    if (wr != 0)
      {
        out << wr << endl;
        return 32;
      }

    erase_pic ();

    QString pk2;
    Config::getToolPath ("pk2", pk2);
    pk2 += " --progress --writeall .pikdev_temp_file.hex 2>&1";

    QByteArray bapk2 = pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");
    char line[500], message[500];

    if (f != 0)
      {
        progress_status = 100;
        while (fgets (line, 499, f) != 0)
	  {
            if (sscanf (line, "[%d%%]", &p100) == 1)
	      progress_status.jump (p100);
            else if (sscanf (line, "Fatal error>%[^\n]", message))
	      {
                clearConsole ();
                addTextConsole (QString (message));
	      }
	  }
        pclose (f);
      }

    remove (".pikdev_temp_file.hex");
    return verify_pic (out);
}

// Read all device memory to internal buffer.
void genericPk2::read_pic ()
{
    QString pk2;
    Config::getToolPath ("pk2", pk2);
    pk2 += " --progress --read .pikdev_temp_file.hex 2>&1";
    int p100;
    progress_status = 100;

    QByteArray bapk2 = pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");
    char line[500], msg[500];

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
            else if (sscanf (line, "[%d%%]", &p100) == 1)
	      progress_status.jump (p100);
	  }
        pclose (f);
      }

    const char *ld = loadHexFile (".pikdev_temp_file.hex");
    if (ld != 0)
      {
        clearConsole ();
        addTextConsole (QString (ld));
      }
    remove (".pikdev_temp_file.hex");
}

// Bulk erase device
int genericPk2::erase_pic ()
{
    int status = 0;

    QString pk2;
    Config::getToolPath ("pk2", pk2);
    pk2 += " --progress --eraseall 2>&1";

    QByteArray bapk2 = pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");
    char line[500], msg[500];

    if (f != 0)
      {
        progress_status = 100;
        while (fgets (line, 499, f) != 0)
	  {
            if (sscanf (line, "Fatal error>%[^\n]", msg) == 1)
	      {
                clearConsole () ;
                addTextConsole (QString (msg));
	      }
	  }
        pclose (f);
        progress_status.jump (100);
      }

    return status;
}

// Probe device. find device name and revision.
bool genericPk2::readDevRev (string& d, string& r)
{
    bool found = false;

    QString pk2;
    Config::getToolPath ("pk2", pk2);
    pk2 += " --progress --device 2>&1";
    char line[500], dev[30], rev[30];
    *dev = 0;
    d = r = "";

    QByteArray bapk2 = pk2.toLatin1 ();
    FILE *f = popen (bapk2, "r");

    if (f != 0)
      {
        progress_status = 100;
        while (fgets (line, 499, f) != 0)
	  {
            // typical message : PIC16F690 Rev 4 found

            if (sscanf (line, "PIC%s Rev %s found", dev, rev) == 2)
	      {
                for (char *p = dev; *p; ++p)
		  {
		    if (isupper (*p))
		      *p = tolower (*p); // sorry ..
		  }
                d = "p" + string (dev);
                r = rev;
                found = true;
	      }
	  }
        pclose (f);
        progress_status.jump (100);
      }

    return found;
}

string genericPk2::read_device_revision ()
{
    string dev, rev, r;

    if (readDevRev (dev, rev))
      r = rev;
    return r;
}

string genericPk2::read_device_name ()
{
    string dev, rev, r;

    if (readDevRev (dev, rev))
      r = dev;
    return r;
}
