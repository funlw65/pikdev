/***************************************************************************
                         genericPkp.cc  -  description
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

#include "Config.h"
#include "genericPkp.h"


genericPkp::genericPkp ()
{
}

// Verify the PIC's memory against memory buffer.
int genericPkp::verify_pic (ostream& out)
{
    // save current buffer to temporary file
    const char *wr = writeHexFile (".pkp_temp_file.hex", "inhx32");
    if (wr != 0)
      {
        cerr << wr << endl;
        return 0; // TODO : implement better error handling
      }

    QString pkp;
    Config::getToolPath ("pkp", pkp);
    pkp += " --mfmode --verify=.pkp_temp_file.hex --device=";
    pkp += specificName ();
    pkp += " 2>&1";


    QByteArray bapkp = pkp.toLatin1 ();

    FILE *f = popen (bapkp, "r");
    char line[500], msg[500];
    int p100;
    int status = 0;

    if (f != 0)
      {
        progress_status = 100;
        while (fgets (line, 499, f) != 0)
        {
            if (sscanf (line, "%d%%", &p100) == 1)
	      progress_status.jump (p100);
            else if (sscanf (line, "pkp:%d:%[^\n]", &status, msg) == 2)
	      out << line;
            else
	      out << line;
        }
        pclose (f);
      }
    return status;
}

// Program RAM buffer into the PIC
int genericPkp::program_pic (ostream& out)
{
    // save current buffer to temporary file
    const char *wr = writeHexFile (".pkp_temp_file.hex", "inhx32");
    if (wr != 0)
      {
        out << wr << endl;
        return 64; // TODO : implement better error handling
      }

    QString pkp;
    Config::getToolPath ("pkp", pkp);
    pkp += " --mfmode --program=.pkp_temp_file.hex --device=";
    pkp += specificName ();
    pkp += " 2>&1";

    QByteArray bapkp = pkp.toLatin1 ();
    FILE *f = popen (bapkp, "r");
    char line[500], msg[500];
    int p100;
    int status = 0;

    if (f != 0)
      {
        progress_status = 100;
        while (fgets (line, 499, f) != 0)
	  {
            if (sscanf (line, "%d%%", &p100) == 1)
	      progress_status.jump (p100);
            else if (sscanf (line, "pkp:%d:%[^\n]", &status, msg) == 2)
	      out << line;
            else
	      out << line;
	  }
        pclose (f);
      }
    return status;
}

// Read all device memory to internal buffer.
void genericPkp::read_pic ()
{
    QString pkp;
    Config::getToolPath ("pkp", pkp);
    pkp += " --mfmode --read=.pkp_temp_file.hex --device=";
    pkp += specificName ();
    pkp += " 2>&1";

    progress_status = 100;

    QByteArray bapkp = pkp.toLatin1 ();
    FILE *f = popen (bapkp, "r");
    char line[500];
    int p100;

    if (f != 0)
      {
        progress_status = 100;
        while (fgets (line, 499, f) != 0)
	  {
            if (sscanf (line, "%d%%", &p100) == 1)
	      progress_status.jump (p100);
	  }
        pclose (f);
      }
    // save current buffer to temporary file
    const char *ld = loadHexFile (".pkp_temp_file.hex");
    if (ld != 0)
      cerr << ld;
}

// Bulk erase device
int genericPkp::erase_pic ()
{
    QString pkp;
    Config::getToolPath ("pkp", pkp);
    pkp += " --mfmode --erase --device=";
    pkp += specificName ();
    pkp += " 2>&1";

    QByteArray bapkp = pkp.toLatin1 ();
    FILE *f = popen (bapkp, "r");
    char line[500], msg[500];
    int status = 0;

    if (f != 0)
      {
        progress_status = 100;
        while (fgets (line, 499, f) != 0)
	  {
            if (sscanf (line, "pkp:%d:%[^\n]", &status, msg) == 2)
	      (void)0; // TODO error handling : NOTE (void)0 avoid spurious warning
	  }
        pclose(f);
        progress_status.jump(100);
      }
    return status;
}

// Probe device. find device name and revision.
bool genericPkp::readDevRev (string& d, string& r)
{
    QString pkp;
    Config::getToolPath ("pkp", pkp);
    pkp += " --mfmode --probe --device=";
    pkp += specificName ();
    pkp += " 2>&1";
    char line[500], dev[30], rev[30];
    *dev = 0;
    bool found = false;
    d = r = "";

    QByteArray bapkp = pkp.toLatin1 ();
    FILE *f = popen (bapkp, "r");

    if (f != 0)
      {
        progress_status = 100;
        while (fgets (line, 499, f) != 0)
	  {
            if (sscanf (line, "probed device: [%[^]]] [%[^]]]", dev, rev) == 2)
	      {
                d = dev;
                r = rev;
                found = true;
	      }
	  }
        pclose (f);
        progress_status.jump (100);
      }
    return found;
}

string genericPkp::read_device_revision ()
{
    string dev, rev, r;

    if (readDevRev (dev, rev))
      r = rev;
    return r;
}

string genericPkp::read_device_name ()
{
    string dev, rev, r;

    if (readDevRev (dev, rev))
      r = dev;
    return r;
}

