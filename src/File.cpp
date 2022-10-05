/***************************************************************************
                          file.cpp  -  description
                             -------------------
    begin                : Tue Jan 27 2004
    copyright            : (C) 2004-2012 by Alain Gibaud
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

#include <cstdio>
#include <iostream>
using namespace std;

#include "codeeditor/QBEditorDocument.h"
#include "File.h"


File::File () : doc (0), current_line (-1), kbuffer (0)
{
}

File::~File ()
{
}

bool File::isConnected ()
{
    return doc != 0;
}

int File::ch ()
{
    if (kbuffer >= (int) buffer.length ())
      {
	if (! getLine ())
	  return EOF;
      }
    return buffer[kbuffer].unicode ();
}

int File::gch ()
{
    int c = ch ();
    if (c != EOF)
      ++kbuffer;
    return c;
}

int File::nch (int offset)
{
    int k = kbuffer + offset;
    if (k >= (int) buffer.length () || k < 0)
      return 0; 
    else
      return buffer[k].unicode ();
}

// true if this line looks like a pre-processor directive
bool File::preprocessor (const QString& s)
{
    int i;
    for(i = 0; s[i] == ' '; ++i)
      ;
    return s[i] == '#';
}

bool File::getLine ()
{
    buffer = "";

    if (++current_line >= (int) doc->blockCount ())
      return false; // end of text

    buffer = doc->findBlockByNumber (current_line).text ();
    buffer += "\n";
    kbuffer = 0;

    // zap preprocessor directives
    // TODO - multi-line preprocessor directive
    if (preprocessor (buffer))
      return getLine ();

    return true;
}

int File::mark (int k)
{
    int old = kbuffer;
    if (k != -1)
      kbuffer = k;
    return old;
}
 
int File::getLineNum ()
{
    return current_line;
}

void File::pushBack (const char *s)
{
    if (! *s)
      return;
    buffer.insert (kbuffer, s);
    kbuffer -= (strlen (s) - 1);
}

void File::connectDocument (QBEditorDocument *d)
{
    doc = d;
}
