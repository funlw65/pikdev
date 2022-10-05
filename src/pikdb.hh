/***************************************************************************
                          pikdb.h  -  description
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
/* @author Gibaud Alain */

#ifndef PIKDB_H
#define PIKDB_H

#include <map>
#include <string>
using namespace std;


struct deviceRecord
{
    const char *gpName; // gpasm name
    unsigned int deviceID;
    const char *configDesc;
    int memaddr1, memaddr2;
    int idaddr1, idaddr2;
    int eeaddr1, eeaddr2;
    int cfaddr1, cfaddr2;
    int calibration1, calibration2;
    int wr_buf_size;
    int core; // 12, 14 or 16
};

class pikDB
{
    map<string, deviceRecord*> gdevices;
    static pikDB *the_db;
    pikDB();
    string gpasmName (const string& name);
  
public: 
    static pikDB *pikdb ();
    ~pikDB ();
    deviceRecord *getData (const char *gpname);
    const char *getName (unsigned int id);
    unsigned int getID (const char *gpname);
};

#endif
