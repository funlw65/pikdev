/***************************************************************************
                          pikdb.cpp  -  description
                             -------------------
    begin                : Tue Jan 10 2006
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

#include "device_data.hh"
#include "pikdb.hh"


pikDB *pikDB::the_db = 0;

pikDB::pikDB ()
{
    // just builds map
    unsigned int i;
    for (i = 0; i < sizeof (devDB) / sizeof (*devDB); ++i)
      {
	string n (devDB[i].gpName);
	gdevices[n] = devDB + i;
      }
}

pikDB::~pikDB ()
{
}

pikDB *pikDB::pikdb ()
{
    if (the_db == 0)
      the_db = new pikDB;
    return the_db;
}

string pikDB::gpasmName (const string& name)
{
    string gpname = "p";
    unsigned int i;

    for (i = 0; i < name.size () && ! isdigit (name[i]); ++i)
      ;
    for (; i < name.size (); ++i)
      gpname += tolower (name[i]);
    return gpname;
}

deviceRecord *pikDB::getData (const char *gpname)
{
    map<string, deviceRecord *>::iterator i;

    i = gdevices.find (string (gpname));
    return i == gdevices.end () ? 0 : (*i).second;
}

/* -----------------------------------------------------------------------
   Get ID for device gpname

   Returns:
   0 : no such device
   -1 : not autodetectable
   otherwise, the device ID, with revision bits masked
   ----------------------------------------------------------------------- */
unsigned int pikDB::getID (const char *gpname)
{
    map<string, deviceRecord *>::iterator i;

    i = gdevices.find (string (gpname));
    return (i == gdevices.end ()) ? 0 : (*i).second->deviceID;  
}

const char *pikDB::getName (unsigned int id)
{
    if (id == 0U || id == -1U)
      return ""; // yes, -1U is legal

    map<string, deviceRecord *>::iterator i;

    for (i = gdevices.begin (); i != gdevices.end (); ++i)
      {
	if ((*i).second->deviceID == id)
	  return (*i).second->gpName;
      }
    return "";
}
