/***************************************************************************
                          file.h  -  description
                             -------------------
    begin                : Tue Jan 27 2004
    copyright            : (C) 2004 by Alain Gibaud
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
/* This class is just an interface to QBEditorDocument for
   management of buffer, push back operations and line numbers tracking
   (used by parser for entry points detection)
   @author Alain Gibaud */

#ifndef FILE_H
#define FILE_H

#include <QString>

#include "codeeditor/QBEditorDocument.h"


class File
{
public:
    File ();
    ~File ();
    bool isConnected ();
    bool getLine ();
    int ch ();
    int gch ();
    int nch (int offset = 1);
    int mark (int k = -1);
    bool preprocessor (const QString& s);
    int getLineNum ();
    // can be used to submit a fake token to the lexer
    void pushBack (const char *s);
    // connect "File" to QBEditorDocument
    void connectDocument (QBEditorDocument *e);

private:
    QBEditorDocument *doc; // input flow
    int current_line;
    QString buffer;
    int kbuffer;
};

#endif
