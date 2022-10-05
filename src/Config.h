/***************************************************************************
                          Config.h  -  description
                             -------------------
    begin                : Sun Dec 21 2003
    copyright            : (C) 2003 by Alain Gibaud
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
/* Management of all the configuration data
   @author Alain Gibaud */

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QStringList>


class Config
{
public:
    Config ();
    ~Config ();
    // get asm configuration in single file mode
    static void getAsmConfig (QString& proc, QString& hex, QString& include, QString& radix,
                              QString& warning, QString& other);

    // set asm config in single file mode
    static void setAsmConfig (const QString& proc, const QString& hex, const QString& include,
			      const QString& radix, const QString& warning, const QString& other);

    // set current programmer config (type, port type and pinout)
    static void setPortConfig (const QString& type, const QString& port, int vpp, int vdd,
			       int clock, int dataout, int datain, int drw, int clkdelay);

    // get current programmer config (type, port type and pinout)
    static void getCurrentPortConfig (QString& type, QString& port, int& vpp, int& vdd,
				      int& clock, int& dataout, int& datain, int& drw,
				      int& clkdelay);

    // get last configured parallel port config
    static void getParPortConfig (QString& port, int& vpp, int& vdd, int& clock, int& dataout,
				  int& datain, int& drw, int& clkdelay);

    // get last configured serial port config
    static void getSerPortConfig (QString& port, int& vpp, int& vdd, int& clock, int& dataout,
				  int& datain, int& drw, int& clkdelay);

    // Write informations about known programming cards
    static void writeStdHardwares ();

    // Returns the list of std known programmers
    static QStringList getStdHardwares ();

    // get pinout and type for "name" programmer
    static void getHardConfig (const QString& name, QString& type, int& vpp, int& vdd, int& clock,
			       int& dataout, int& datain, int& drw, int& clkdelay);

    // get pinout and type for "name" programmer
    static void setHardConfig (const QString& name, const QString& type, int vpp, int vdd,
			       int clock, int dataout, int datain, int drw, int clkdelay);

    // get disassmbler configuration
    static void getDAsmConfig (QString& proc, bool& shortlist, QString& other);

    // get disassmbler configuration
    static void setDAsmConfig (const QString& proc, bool shortlist, const QString& other);

    // get current version
    static void getGlobal (QString& version);

    // set current version
    static void setGlobal (const QString& version);

    // Saves the currently opened project and files names
    static void setOpened (const QString& project, const QStringList& files);

    // Reads the previously opened project and files names for restauration purpose
    static void getOpened (QString& project, QStringList& files);

    // get current programmer name
    static void getCurrentProgrammer (QString& name);

    // set current version
    static void setCurrentProgrammer (const QString& name);

    // get known path for an external tool (if unknown, try to locate tool)
    static void getToolPath (const QString& toolname, QString& path);

    // get path for an external tool
    static void setToolPath (const QString& toolname, const QString& path);

    // create a tool entry
    static void createToolEntry (const QString& toolname, const QString& info, const QString& URL,
				 bool mand);

    // get a tool entry
    static void getToolEntry (const QString& toolname, QString& path, QString& info, QString& URL,
			      bool& mand);

    // get the list of external tools
    static void getToolNames (QStringList& namelist);

    static void removeToolList ();
};

#endif
