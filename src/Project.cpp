/***************************************************************************
                          Project.cpp  -  description
                             -------------------
    begin                : Thu Jun 3 2003
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

#include <iostream>
using namespace std;

#include <QDateTime>
#include <QFileInfo>
#include <QStringList>

#include "FNU.h"
#include "Project.h"


Project::Project (const QString& projectfile)
{
    config = new QSettings (projectfile, QSettings::IniFormat);
    if (config)
      projectFileName = projectfile;
}

Project::~Project ()
{
    if (config)
      delete config; // Note: sync () is automatic
}

// Add a new file to this project
void Project::addFile (const QString& filename)
{
    QStringList files = getFiles ();
    files += filename;
    config->setValue ("Files/inputFiles", files);
    config->sync ();
}

// Remove a file from the project
void Project::removeFile (const QString& filename)
{
    QStringList files = getFiles ();
    files.removeOne (filename);
    config->setValue ("Files/inputFiles", files);
    config->sync ();
}

/* Returns the project base directory (extracted from the project file name)
   with a trailing "/" */
QString Project::projectPath ()
{
    QFileInfo fi = projectFileName;
    return fi.absolutePath () + "/";
}

// Set the short description for this project
void Project::setDescription (const QString& desc)
{
    config->setValue ("description", desc);
    config->sync ();
}

// Set the version id for this project
void Project::setVersion (const QString& ver)
{
    config->setValue ("version", ver);
    config->sync ();
}

// Set the type for this project
void Project::setType (const QString& t)
{
    config->setValue ("type", t);
    config->sync ();
}

// Set the output file (.hex) for this project
void Project::setOutput (const QString& out)
{
    config->setValue ("output", out);
    config->sync ();
}

// Simplify a path + file name, to make it relative to the project filename
QString Project::projectRelative (const QString& filename)
{
    QString pp = projectPath (), r = filename;
    int i = r.indexOf (pp);
    if (i == 0) // project path found at the begining of filename
      r = r.right (filename.length () - pp.length ());
    return r;
}

/* Generates an absolute filename from a project relative name
   (absolute names remain unchanged) */
QString Project::absFilename (const QString &fn)
{
    return (fn[0] == '/') ? fn : projectPath () + fn;
}

// Returns the list of files used in the project
QStringList Project::getFiles ()
{
    return config->value ("Files/inputFiles").toStringList ();
}

// Returns the list of absolute file names used in the project
QStringList Project::getAbsFiles ()
{
    QString proj_path = FNU::path (projectFileName), f;
    QStringList files (getFiles ()), abs_files;

    for (QStringList::iterator i = files.begin (); i != files.end (); ++i)
      abs_files += absFilename (*i);
    return abs_files;
}

// Returns the project name (i.e.: naked project file name)
QString Project::projectName ()
{
    return FNU::file (projectFileName);
}

/* Returns true if absolute filename af is already in the project
   filename list. */
bool Project::absFileExists (const QString& af)
{
    QStringList files (getAbsFiles ());
    for (QStringList::iterator i = files.begin (); i != files.end (); ++i)
      {
        if (*i == af)
	  return true;
      }
    return false;
}

QString Project::getType ()
{
    return config->value ("type", "asm").toString ();
}

QString Project::getDescription ()
{
    return config->value ("description", "<none>").toString ();
}

QString Project::getVersion ()
{
    return config->value ("version", "0.1").toString ();
}

QString Project::getDevice ()
{
    return config->value ("Assembler/target-device", "p12c508").toString ();
}

void Project::setDevice (const QString& x)
{
    config->setValue ("Assembler/target-device", (x));
}

QString Project::getRadix ()
{
    return config->value ("Assembler/radix", "dec").toString ();
}

void Project::setRadix (const QString& x)
{
    config->setValue ("Assembler/radix", (x));
}

QString Project::getWarnLevel ()
{
    return config->value ("Assembler/warn-level", "2").toString ();
}

void Project::setWarnLevel (const QString& x)
{
    config->setValue ("Assembler/warn-level", x);
}

QString Project::getIncludeDir ()
{
    return config->value ("Assembler/include-dir", "").toString ();
}

void Project::setIncludeDir (const QString& x)
{
    config->setValue ("Assembler/include-dir", x);
}

QString Project::getCIncludeDir ()
{
    return config->value ("C/include-dir", "").toString ();
}

void Project::setCIncludeDir (const QString& x)
{
    config->setValue ("C/include-dir", x);
}

QString Project::getCCompiler ()
{
    return config->value ("C/compiler", "cpik").toString ();
}

/* For compatibility with old project files
   the config values list is saved as multiple different keys
   rather than an indexed key */
void Project::setConfigWords (const QVector<int>& words)
{
    QString w = "ConfigWords/CONFIG%1";

    config->setValue ("ConfigWords/WordsCount", words.size ());

    for (int i = 0; i < words.size (); ++i)
      {
        config->setValue (w.arg (i + 1), QString::asprintf ("%X", words[i])); // eg: CONFIG3=E2
      }
    config->sync ();
}

QVector<int> Project::getConfigWords ()
{
    QString w = "ConfigWords/CONFIG%1";
    int nw;
    QVector<int> wv;

    nw = config->value ("ConfigWords/WordsCount", 0).toInt ();

    for (int i = 0; i < nw; ++i)
      {
        QString e = config->value (w.arg (i + 1), "0").toString ();
        bool ok;
        int value = e.toInt (&ok, 16);
        wv.push_back (ok ? value : 0);
      }
    return wv;
}

void Project::setCCompiler (const QString& x)
{
    config->setValue ("C/compiler", x);
}

QString Project::getCLibsDir ()
{
    return config->value ("C/libs-dir", "").toString ();
}

void Project::setCLibsDir (const QString& x)
{
    config->setValue ("C/libs-dir", x);
}

QString Project::getOtherOptions ()
{
    return config->value ("Assembler/other-options", "").toString ();
}

void Project::setOtherOptions (const QString& x)
{
    config->setValue ("Assembler/other-options", x);
}

QString Project::getOtherCOptions ()
{
    return config->value ("C/other-options", "").toString ();
}

void Project::setOtherCOptions (const QString& x)
{
    config->setValue ("C/other-options", x);
}

bool Project::getCSlb ()
{
    return config->value ("C/slb", false).toBool ();
}

void Project::setCSlb (bool x)
{
    config->setValue ("C/slb", x);
}

bool Project::getCJmpOpt ()
{
    return config->value ("C/jmpopt", false).toBool ();
}

void Project::setCJmpOpt (bool x)
{
    config->setValue ("C/jmpopt", x);
}

bool Project::getCAsm ()
{
    return config->value ("C/asm", false).toBool ();
}

void Project::setCAsm (bool x)
{
    config->setValue ("C/asm", x);
}

bool Project::getAsmList ()
{
    return config->value ("Assembler/list", false).toBool ();
}

void Project::setAsmList (bool x)
{
    config->setValue ("Assembler/list", x);
}

bool Project::getLinkDebug ()
{
    return config->value ("Linker/debug", false).toBool ();
}

void Project::setLinkDebug (bool x)
{
    config->setValue ("Linker/debug", x);
}

bool Project::getLinkMap ()
{
    return config->value ("Linker/map", false).toBool ();
}

void Project::setLinkMap (bool x)
{
    config->setValue ("Linker/map", x);
}

QString Project::getHexFormat ()
{
    return config->value ("Linker/hex-format", "inhx32").toString ();
}

void Project::setHexFormat (const QString& x)
{
    config->setValue ("Linker/hex-format", x);
}

QString Project::getLibsDir ()
{
    return config->value ("Linker/objs-libs-dir", "").toString ();
}

void Project::setLibsDir (const QString& x)
{
    config->setValue ("Linker/objs-libs-dir", x);
}

// Linker script file
QString Project::getLinkScript ()
{
    return projectPath () + config->value ("Linker/linker-script", "").toString ();
}
void Project::setLinkScript (const QString& s)
{
    config->setValue ("Linker/linker-script", s);
}

QString Project::getOtherLinkOptions ()
{
    return config->value ("Linker/other-options", "").toString ();
}

void Project::setOtherLinkOptions (const QString& x)
{
    config->setValue ("Linker/other-options", x);
}

QString Project::projectPathFileName ()
{
    return projectFileName;
}
// Compute hex file name from project file name
QString Project::getHexName ()
{
    return FNU::changeExt (projectPathFileName (), "hex");
}

// Compute asm file name from project file name
QString Project::getAsmName ()
{
    return FNU::changeExt (projectPathFileName (), "asm");
}

// true if the project contains C source/header file(s).
bool Project::isCproject ()
{
    return getType () == "C";
}

void Project::commit ()
{
    config->sync ();
}
