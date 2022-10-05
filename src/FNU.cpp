/***************************************************************************
                          FNU.cpp  -  description
                             -------------------
    begin                : Tue Dec 2 2003
    copyright            : (C) 2003 by Alain Gibaud
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

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QLatin1String>
#include <QRegExp>

#include "FNU.h"


bool FNU::isAsmSource (const QString& fname)
{
    QString e (ext (fname));
    return e == "src" || e == "asm" || e == "pic" || e == "SRC" || e == "ASM" || e == "PIC";
}

bool FNU::isObjFile (const QString& fname)
{
    QString e (ext (fname));
    return e == "o" || e == "O";
}

bool FNU::isLibFile (const QString& fname)
{
    QString e (ext (fname));
    return e == "lib" || e == "a" || e == "LIB" || e == "A";
}

bool FNU::isProjectFile (const QString& fname)
{
    QString e (ext (fname));
    return e == "pikprj";
}

bool FNU::isIncFile (const QString& fname)
{
    QString e (ext (fname));
    return e == "inc" || e == "INC";
}

bool FNU::isCSource (const QString& fname)
{
    QString e (ext (fname));
    return e == "c" || e == "C";
}

bool FNU::isCHeader (const QString& fname)
{
    QString e (ext (fname));
    return e == "h" || e == "H";
}

bool FNU::isSrcLib (const QString& fname)
{
    QString e (ext (fname));
    return e == "slb" || e == "SLB";
}

bool FNU::isHexFile (const QString& fname)
{
    QString e (ext (fname));
    return e == "hex" || e == "HEX";
}

bool FNU::isLnkScrFile (const QString& fname)
{
    QString e (ext (fname));
    return e == "lkr" || e == "LKR";
}

bool FNU::isRelative (const QString& fname)
{
    return fname[0] != '/';
}

QString FNU::asmName2HexName (const QString& f)
{
    QString n = "";
    if (isAsmSource (f))
      n = changeExt (f, "hex");
    return n;
}

QString FNU::CName2SlbName (const QString& f)
{
    QString n = "";
    if (isCSource (f))
      n = changeExt (f, "slb");
    return n;
}

QString FNU::asmName2ObjName (const QString& f)
{
    QString n = "";
    if (isAsmSource (f))
      n = changeExt (f, "o");
    return n;
}

// Compute the list filename from the asm filename
QString FNU::asmName2LstName (const QString& f)
{
    QString n = "";
    if (isAsmSource (f))
      n = changeExt (f, "lst");
    return n;
}

// Compute the hex filename from the asm filename
QString FNU::hexName2AsmName (const QString& f)
{
    QString n = "";
    if (isHexFile (f))
      n = changeExt (f, "asm");
    return n;
}

// Returns the filename extension
QString FNU::ext (const QString& f)
{
    QFileInfo fi (f);
    QString fname (fi.fileName ());
    int i = fname.lastIndexOf ('.');

    return i == -1 ? QString ("") : fname.mid (i + 1);
}

// Returns the naked filename (no extension, no path)
QString FNU::file (const QString& f)
{
    QFileInfo fi (f);
    QString fname (fi.fileName ());
    int i = fname.lastIndexOf ('.');

    return i == -1 ? fname : fname.left (i);
}

// Returns filename + extension
QString FNU::fileExt (const QString& f)
{
    QFileInfo fi (f);
    return fi.fileName ();
}

// Returns path only
QString FNU::path (const QString& f)
{
    int i = f.lastIndexOf ('/');
    QString r (""); // no path
    if (i == 0)
      r = "/";    // file on /
    else if (i != -1)
      r = f.left (i);
    return r;
}

// Returns dir + filename (no extension)
QString FNU::pathFile (const QString& f)
{
    QString p (path (f)), r = "";
    if (p == "/")
      r = "/" + file (f);
    else if (p == "")
      r = file (f);
    else
      r = p + "/" + file(f);
    return r;
}

/* Returns true if fname is an up to date source filename
   (i.e.: fname is a source filename and
   date (oject file) > date (source file) */
bool FNU::isUpToDate (__attribute__ ((unused)) const QString& fname)
{
    // As this feature needs more file dependency
    // study, I prefer to force asm by 	
    // claiming object file is not up-to-date
    return false;
}

QString FNU::changeExt (const QString& f, const QString& ext)
{
    QString n;
    int k = f.lastIndexOf ('.', f.length () - 1);
    if (k != -1)
      {
        n = f.left (k);
        n += ".";
        n += ext;
      }
    return n;
}

/* Try to  find an application with the command "which"
   Returns:
   true if this application is known */
#if 0
int FNU::appFound (const QString& cmd, QString& fullpath)
{
    char path[256];
    fullpath = "";
    FILE *fp = popen (("which " + cmd + " 2>&1").latin1 (), "r");
    if (fp == 0)
      return 0;
    *path = 0;
    int count;
    for (count = 0; fscanf (fp, "%255s", path) > 0; ++count)
      fullpath += path;

    int status = pclose (fp);
    if (status == 0)
      return count; // which return 0 if prg found
    return 0;
}
#endif

void FNU::test (const QString& x)
{
    std::cout << "x= " << x.toStdString () << std::endl;
    std::cout << "path= " << path (x).toStdString () << std::endl;
    std::cout << "file= " << file (x).toStdString () << std::endl;
    std::cout << "ext= " << ext (x).toStdString () << std::endl;
    std::cout << "pathFile= " << pathFile (x).toStdString () << std::endl;
    std::cout << "fileExt= " << fileExt (x).toStdString () << std::endl;
    std::cout << "--------------" << std::endl << std::flush;
}

// remove any leading file:// from the file name
QString FNU::noScheme (const QString& n)
{
    QString r (n);
    if (r.startsWith ("file://"))
      r.remove (0, 7);

    return r;
}

// remove any leading file:// from the file name
QString FNU::noScheme (const QUrl& u)
{
    return noScheme (u.toString ());
}
