/***************************************************************************
                          Project.h  -  description
                             -------------------
    begin                : Thu Jun 3 2003
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
/* A class to organize project data
   @author Alain Gibaud */

#ifndef PROJECT_H
#define PROJECT_H

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVector>


class Project
{
public:
    // Creates an empty project
    Project (const QString& projectfile);
    ~Project ();

    // Returns the project base directory (extracted from the project file name)
    QString projectPath ();

    // Simplify a path + file name, to make it relative to the project filename
    QString projectRelative (const QString& filename);

    // Returns the list of file names used int the project
    QStringList getFiles ();

    // Returns the list of absolutes file names used int the project
    QStringList getAbsFiles ();

    /* Returns the project name
       (i.e.: naked project file name) */
    QString projectName ();

    // generates absolute filename from project relative filename
    QString absFilename (const QString&);

    /* Returns true if absolute filename af is already in the project
       filename list. */
    bool absFileExists (const QString& af);

    QString getVersion ();
    QString getType ();
    QString getDescription ();
    void setDescription (const QString&);
    QString getDevice ();
    void setDevice (const QString&);
    QString getHexFormat ();
    void setHexFormat (const QString&);
    QString getRadix ();
    void setRadix (const QString&);
    QString getWarnLevel ();
    void setWarnLevel (const QString&);
    QString getOtherOptions ();
    void setOtherOptions (const QString&);
    QString getIncludeDir ();
    void setIncludeDir (const QString&);
    QString getCIncludeDir ();
    void setCIncludeDir (const QString&);
    QString getCLibsDir ();
    void setCLibsDir (const QString&);
    void setConfigWords (const QVector<int>& words);
    QVector<int> getConfigWords ();
    QString getCCompiler ();
    void setCCompiler (const QString&);
    bool getCSlb ();
    void setCSlb (bool);
    bool getCJmpOpt ();
    void setCJmpOpt (bool);
    bool getCAsm ();
    void setCAsm (bool);
    bool getLinkDebug ();
    void setLinkDebug (bool);
    bool getLinkMap ();
    void setLinkMap (bool);
    bool getAsmList ();
    void setAsmList (bool);
    QString getLibsDir ();
    void setLibsDir (const QString&);

    // Linker script file
    QString getLinkScript ();
    void setLinkScript (const QString&);

    QString getOtherLinkOptions ();
    void setOtherLinkOptions (const QString&);

    QString getOtherCOptions ();
    void setOtherCOptions (const QString&);

    QString projectPathFileName ();
    // Compute hex file name from project file name
    QString getHexName ();
    // Compute asm file name from project file name
    QString getAsmName ();
    // true if the project contains C source file(s).
    bool isCproject ();

    // Remove a file from the project
    void removeFile (const QString& filename);
    // Add a new file to this project
    void addFile (const QString& filename);
    // Set the version id for this project
    void setVersion (const QString& id);
    // Set the type (asm or C) for this project
    void setType (const QString& t);
    // Set the output (hex) file for this project
    void setOutput (const QString& file);
    // flush data to file
    void commit ();

private:
    // The configuration object which manages the project data
    QSettings *config;
    // The name of the .pikprj file
    QString projectFileName;
};

#endif
