/***************************************************************************
                          FNU.h  -  description
                             -------------------
    begin                : Tue Dec 2 2003
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
/* FNU = File Name Utilities
   Functions for file name manipulation
   @author Alain Gibaud */

#ifndef FNU_H
#define FNU_H

#include <QString>
#include <QUrl>


class FNU
{
public:
    static QString asmName2HexName (const QString&);
    static QString asmName2LstName (const QString&);
    static QString hexName2AsmName (const QString&);
    static QString asmName2ObjName (const QString&);
    static QString CName2SlbName (const QString&);
    static bool isAsmSource (const QString&);
    static bool isCSource (const QString&);
    static bool isCHeader (const QString&);
    static bool isSrcLib (const QString&);
    static bool isObjFile (const QString&);
    static bool isLibFile (const QString&);
    static bool isHexFile (const QString&);
    static bool isProjectFile (const QString& name);
    static bool isIncFile (const QString& name);
    static bool isLnkScrFile (const QString& name);
    static bool isRelative (const QString&);
    static bool isUpToDate (const QString& fname);
    static QString ext (const QString&);
    static QString path (const QString&);
    static QString file (const QString&);
    static QString fileExt (const QString&);
    static QString pathFile (const QString&);
    static QString changeExt (const QString& n, const QString& ext);
    static QString noScheme (const QString& n); // remove file://
    static QString noScheme (const QUrl& u);  // idem : convenience function
    // static int appFound (const QString&, QString&);
    static void test (const QString&);
};

#endif
