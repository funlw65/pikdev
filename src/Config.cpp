/***************************************************************************
                          Config.cpp  -  description
                             -------------------
    begin                : Sun Dec 21 2003
    copyright            : (C) 2003-2012 by Alain Gibaud
    email                : alain.gibaud@univ-valenciennes.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
using namespace std;

#include <QSettings>

#include "Config.h"
#include "FNU.h"


Config::Config ()
{
}

Config::~Config ()
{
}

// ASM group
void Config::getAsmConfig (QString& proc, QString& hex, QString& include, QString& radix,
                           QString& warning, QString& other)
{
    QSettings confg;

    proc = confg.value ("asm/proc", "p12c508").toString ();
    hex = confg.value ("asm/hexfmt", "inhx32").toString ();
    include = confg.value ("asm/include", "$(SRCPATH)").toString ();
    radix = confg.value ("asm/radix", "dec").toString ();
    warning = confg.value ("asm/warning", "2").toString ();
    other = confg.value ("asm/others", "").toString ();
}

void Config::setAsmConfig (const QString& proc, const QString& hexformat,const QString& include,
			   const QString& radix, const QString& warnlevel, const QString& others)
{
    QSettings confg;

    confg.setValue ("asm/proc", proc);
    confg.setValue ("asm/hexfmt", hexformat);
    confg.setValue ("asm/include", include);
    confg.setValue ("asm/radix", radix);
    confg.setValue ("asm/warning", warnlevel);
    confg.setValue ("asm/others", others);
}

// port configuration (hardware group)
void Config::getCurrentPortConfig (QString& type, QString& port, int& vpp, int& vdd,
                                   int& clock, int& dataout, int& datain, int& drw,
				   int& clkdelay)
{
    QSettings confg;

    type = confg.value ("hardware/port-type", "parallel").toString ();
    if (type == "parallel")
      getParPortConfig (port, vpp, vdd, clock, dataout, datain, drw, clkdelay);
    else if (type == "serial") // serial port
      getSerPortConfig (port, vpp, vdd, clock, dataout, datain, drw, clkdelay);
}

void Config::setPortConfig (const QString& type, const QString& port, int vpp,
                            int vdd, int clock, int dataout, int datain, int drw,
			    int clkdelay)
{
    QSettings conf;

    conf.setValue ("hardware/port-type", type);
    conf.setValue ("hardware/device", port);
    conf.setValue ("hardware/vpp", vpp);
    conf.setValue ("hardware/vdd", vdd);
    conf.setValue ("hardware/clock", clock);
    conf.setValue ("hardware/datao", dataout);
    conf.setValue ("hardware/datai", datain);
    conf.setValue ("hardware/drw", drw);
    conf.setValue ("hardware/clkdelay", clkdelay);
    conf.sync ();
}

void Config::getParPortConfig (QString& port, int& vpp, int& vdd, int& clock,
                               int& dataout, int& datain, int& drw, int& clkdelay)
{
    QSettings conf;

    clock = conf.value ("parallel/clock", 3).toInt ();
    datain = conf.value ("parallel/datai", 10).toInt ();
    dataout = conf.value ("parallel/datao", 2).toInt ();
    // Default RW Line not used...
    drw = conf.value ("parallel/drw", 25).toInt ();  // 25 = GND
    vdd = conf.value ("parallel/vdd", -4).toInt ();
    vpp = conf.value ("parallel/vpp", -5).toInt ();
    clkdelay = conf.value ("parallel/clkdelay", 0).toInt ();
    port = conf.value ("parallel/device", "/dev/parport0").toString ();
}

void Config::getSerPortConfig (QString& port, int& vpp, int& vdd, int& clock,
                               int& dataout, int& datain, int& drw, int& clkdelay)
{
    QSettings conf;

    // JDM
    clock = conf.value ("serial/clock", 7).toInt ();
    datain = conf.value ("serial/datai", 8).toInt ();
    dataout = conf.value ("serial/datao", 4).toInt ();
    // Default RW Line not used...
    drw = conf.value ("serial/drw", 5).toInt (); // 5 = GND
    vdd = conf.value ("serial/vdd", 5).toInt ();
    vpp = conf.value ("serial/vpp", 3).toInt ();
    clkdelay = conf.value ("serial/clkdelay", 0).toInt ();
    port = conf.value ("serial/device", "/dev/ttyS0").toString ();
}

// Write informations about known standard programming cards
void Config::writeStdHardwares ()
{
    // int vpp, int vdd, int clock, int dataout, int datain, int drw;
    setHardConfig ("Tait classic", "parallel", -5, -4, 3, 2, 10, 25, 0);
    setHardConfig ("Tait 7405/7406", "parallel", 5, 4, -3, -2, -10, 25, 0);
    setHardConfig ("P16PRO40 classic", "parallel", 5, 4, -3, -2, -10, 25, 0);
    setHardConfig ("P16PRO40 7407", "parallel", -5, -4, 3, 2, 10, 25, 0);
    setHardConfig ("P16PRO40-VPP40 classic", "parallel", 6, 4, -3, -2, -10, 25, 0);
    setHardConfig ("P16PRO40-VPP40 7407", "parallel", -6, -4, 3, 2, 10, 25, 0);
    setHardConfig ("EPIC+ (You must disconnect 7407 pin 2)", "parallel", -5, -4, 3, 2,
		   10, 25, 0);
    setHardConfig ("JDM classic", "serial", 3, 5, 7, 4, 8, 5, 0);
    setHardConfig ("JDM classic (delay 10)", "serial", 3, 5, 7, 4, 8, 5, 10);
    setHardConfig ("JDM classic (delay 20)", "serial", 3, 5, 7, 4, 8, 5, 20);
    setHardConfig ("PIC Elmer", "serial", -3, 5, -7, -4, -8, 5, 0);
    setHardConfig ("Velleman K8048", "serial", -3, 5, -7, -4, -8, 5, 0);

    /* Added by Mirko Panciri 10/03/2004...
       Visit http://www.pic-tools.com
       I have tested only the "Asix Piccolo" version...
       I think the lines is the same of "Asix Piccolo Grande"...
       Who have "Asix Piccolo Grande" contact panciom@libero.it
       for info / result of test!!! */
    setHardConfig ("Asix Piccolo", "parallel", -6, -7, -5, -3, -10, -2, 0);
    setHardConfig ("Asix Piccolo Grande", "parallel", -6, -7, -5, -3, -10, -2, 0);
    setHardConfig ("Propic2 Vpp-1", "parallel", -5, -4, 3, 2, 10, 25, 0);
    setHardConfig ("Propic2 Vpp-2", "parallel", -6, -4, 3, 2, 10, 25, 0);
    setHardConfig ("Propic2 Vpp-3", "parallel", -7, -4, 3, 2, 10, 25, 0);
    setHardConfig ("Myke's EL Cheapo", "parallel", 16, 25, -1, -17, 13, 25, 0);
    setHardConfig ("EL Cheapo classic (not tested)", "parallel", 16, 25, 1, 17, -13, 25, 0);
    setHardConfig ("Monty-Robot programmer", "parallel", -5, 4, 2, 3, 10, 25, 0);
    setHardConfig ("HOODMICRO (k9spud.com/hoodmicro/)", "serial", 4, 5, 7, 3, 8, 5, 0);
}

void Config::setHardConfig (const QString& name, const QString& type, int vpp,
                            int vdd, int clock, int dataout, int datain, int drw,
			    int clkdelay)
{
    QStringList h = Config::getStdHardwares ();
    QSettings conf;

    conf.beginGroup ("hardware");

    if (h.indexOf (name) == -1)
      {
	// update the list of known hardwares
        h += name;
        conf.setValue ("list", h);
      }
    
    // Add data for this hardware
    conf.beginGroup ("descriptions");
    conf.beginGroup (name);
    conf.setValue ("vpp", vpp);
    conf.setValue ("vdd", vdd);
    conf.setValue ("clock", clock);
    conf.setValue ("datao", dataout);
    conf.setValue ("datai", datain);
    conf.setValue ("port-type", type);    
    conf.setValue ("drw", drw);    
    conf.setValue ("clkdelay", clkdelay);
    conf.endGroup ();
    conf.endGroup ();
    conf.endGroup ();
}

// Return the list of std known programmers
QStringList Config::getStdHardwares ()
{
    QSettings conf;
    
    return conf.value ("hardware/list").toStringList ();
}

// Read the pin configuration for the hardware "name"
void Config::getHardConfig (const QString& name, QString& type, int& vpp, int& vdd,
                            int& clock, int& dataout, int& datain, int& drw, int& clkdelay)
{
    QSettings conf;

    conf.beginGroup ("hardware");
    conf.beginGroup ("descriptions");
    conf.beginGroup (name);
    // returned values are defaulted to Tait if the specified hardware does not exist
    vpp = conf.value ("vpp", -5).toInt ();
    vdd = conf.value ("vdd", -4).toInt ();
    clock = conf.value ("clock", 3).toInt ();
    dataout = conf.value ("datao", 2).toInt ();
    drw = conf.value ("drw", 25).toInt ();
    datain = conf.value ("datai", 10).toInt ();
    type = conf.value ("port-type", "parallel").toString ();
    clkdelay = conf.value ("clkdelay", 0).toInt ();
    conf.endGroup ();
    conf.endGroup ();
    conf.endGroup ();
}

// end of port configuration stuff
void Config::getDAsmConfig (QString& proc, bool& shortlist, QString& other)
{
    QSettings conf;

    conf.beginGroup ("dasm");
    proc = conf.value ("proc", "p12c508").toString ();
    shortlist = (bool) conf.value ("shortlist", 1).toInt ();
    other = conf.value ("others", "").toString ();
    conf.endGroup ();
}

void Config::setDAsmConfig (const QString& proc, bool shortlist, const QString& other)
{
    QSettings conf;

    conf.beginGroup("dasm");
    conf.setValue ("proc", proc);
    conf.setValue ("shortlist", shortlist);
    conf.setValue ("others", other);
    conf.endGroup ();
}

void Config::getGlobal (QString& version)
{
    QSettings conf;

    version = conf.value ("global-settings/version", "<Unknown>").toString ();
}

void Config::setGlobal (const QString& version)
{
    QSettings conf;
    
    conf.setValue ("global-settings/version", version);
}

// Saves the currently opened project and files names
void Config::setOpened (const QString& project, const QStringList& files)
{
    QSettings conf;

    conf.beginGroup ("opened-files");
    conf.setValue ("project", project);
    conf.setValue ("files", files);
    conf.endGroup ();
}

// Reads the previously opened project and files names for restauration purpose
void Config::getOpened (QString& project, QStringList& files)
{
    QSettings conf;

    conf.beginGroup ("opened-files");
    project = conf.value ("project", "").toString ();
    files = conf.value ("files").toStringList ();
    conf.endGroup ();
}

// get current programmer name
void Config::setCurrentProgrammer (const QString& name)
{
    QSettings conf;

    conf.setValue ("global-settings/programmer-engine", name);
}

// set current version
void Config::getCurrentProgrammer (QString& name)
{
    QSettings conf;

    name = conf.value ("global-settings/programmer-engine", "pk2cmd").toString ();
}

// get current tool path for tool
void Config::getToolPath (const QString& toolname, QString& path)
{
    QSettings conf;

    conf.beginGroup ("xtools");
    path = conf.value (toolname + "/path", "").toString ();
    conf.endGroup ();
}

// set tool path for toolname
void Config::setToolPath (const QString& toolname, const QString& path)
{
    QSettings conf;

    conf.beginGroup ("xtools");
    conf.setValue (toolname + "/path", path);
    conf.endGroup ();
}

/* create a tool entry, try to locate executable
   but keep old location if already defined */
void Config::createToolEntry (const QString& toolname, const QString& info,
                              const QString& URL, bool mand)
{
    QSettings conf;

    QStringList toollist = conf.value ("xtools/list").toStringList ();

    if (toollist.indexOf (toolname) == -1)
      {
        // does not exist
        toollist += toolname;
        conf.setValue ("xtools/list", toollist);
      }

    conf.beginGroup ("xtools");
    conf.beginGroup (toolname);
    conf.setValue ("info", info);
    conf.setValue ("URL", URL);
    conf.setValue ("mandatory", mand);

#if 0
    // try to locate tool, if path does not exist
    QString path = conf.value ("path", "").toString ();
    if (path == "" && (FNU::appFound (toolname, path) == 1))
      conf.setValue ("path", path);
#else
    /* because it is virtually impossible to locate the "good" version of a tool
       path must be manually set by user. The FNU::appFound function should not be
       used anymore. */
#endif

    conf.endGroup ();
    conf.endGroup ();
}

// get a tool entry
void Config::getToolEntry (const QString& toolname, QString& path, QString& info,
			   QString& URL, bool& mand)
{
    path = info = URL = "";
    QSettings conf;
    QStringList toollist = conf.value ("xtools/list").toStringList ();

    if (toollist.indexOf (toolname) == -1)
      return; // tool does not exist

    conf.beginGroup ("xtools");
    conf.beginGroup (toolname);

    path = conf.value ("path", "").toString ();
    info = conf.value ("info", "").toString ();
    URL = conf.value ("URL", "").toString ();
    mand = (bool) conf.value ("mandatory", 0).toInt ();

    conf.endGroup ();
    conf.endGroup ();
}

void Config::getToolNames (QStringList& namelist)
{
    QSettings conf;

    namelist.clear ();
    namelist = conf.value ("xtools/list").toStringList ();
}

void Config::removeToolList ()
{
    QSettings config;

    config.setValue ("xtools/list", QStringList ());
}
