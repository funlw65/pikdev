/***************************************************************************
                          lexer.h  -  description
                            -------------------
    begin                : Sat Nov 20 2004
    copyright            : (C) 2004 by Gibaud Alain
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
/* A simple lexer for reading C source files.
   @author Gibaud Alain */

#ifndef LEXER_H
#define LEXER_H

#include <string>
using namespace std;

#include "File.h"


class Lexer
{
public: 
    Lexer ();
    ~Lexer ();
    // connect Lexer to input stream and Flatten stream
    void connectDocument (QBEditorDocument *f);

    // Exact match
    bool match (const char *a_string);
    // case insensitive exact match
    bool cumatch (const char *a_string);
    // true if chars are equal (case insensitive)
    bool cueq (char, char);
    // Match a keyword : ie exact match not followed by AN char
    bool kmatch (const char *keyword);
    bool blk ();
    bool spc ();
    // look for any blanks
    bool needblk ();
    // look at blanks (excluding newlines)
    bool needspc ();
    // peek current char
    int ch ();
    // peek n'th char
    int nch (int n);
    // peek current char and discard it from input
    int gch ();
    // get/set current position
    int mark (int k = -1);
    // abort current line parsing
    bool zap (char final = '\n');
    // abort current instruction parsing
    bool zapInstr () { return zap (';'); }
    // look for a valid C identifier, returns it
    bool ident (string& id);
    // return true if c is a blank char
    bool xisblank (int c);
    // low level parsing tools
    int digit (int c, int b);
    bool xnumber (int& val, int base = 16);
    bool number (int& val);
    bool xisalnum (int c);
    bool xisalpha (int c);
    // Fetch nex n chars (do not use for parser)
    string nextChars (int n);
    /* Convert an escape sequence to an unique code
       (string an char litterals parsing) */
    int esc_seq ();
    int getLineNum ();
    // Can be used to pass a fake token to the lexer
    void pushBack (const char *s);
    bool opMatch (const char *op);
    bool matchReject (const char *s, const char *r);
  
private:
    File _source;
};

#endif
