/***************************************************************************
                          lexer.cpp  -  description
                             -------------------
    begin                : Sat Nov 20 2004
    copyright            : (C) 2004-2012 by Gibaud Alain
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
#include <string>
using namespace std;

#include "Lexer.h"
#include "codeeditor/QBEditorDocument.h"


Lexer::Lexer ()
{
}

Lexer::~Lexer ()
{
}

void Lexer::connectDocument (QBEditorDocument *d)
{
    _source.connectDocument (d);
}


// true is input matches "str" string (case sensitive)
bool Lexer::match (const char *str)
{
    int pos = mark ();

    while ((char) ch () == *str)
      {
	gch ();
	++str;
      }
    if (*str)
      {
	mark (pos);
	return false;
      }
    return true;
}

bool Lexer::cueq (char model, char candidate) 
{
    return ((model == candidate) || (tolower (model) == tolower (candidate)));
}

// true is input matches "str" string (case unsensitive)
bool Lexer::cumatch (const char *str)
{
    int pos = mark ();

    while (cueq ((char) ch (), *str))
      {
	gch ();
	++str;
      }
    if (*str)
      {
	mark (pos);
	return false;
      }
    return true;
}

bool Lexer::needblk ()
{
    char c;
    bool found;
    for (found = false; (c = (char) ch ()) == ' '|| c == '\n' || c == '\t'; found = true)
      gch ();
    return found;
}

bool Lexer::blk ()
{
    needblk ();
    return true;
}

bool Lexer::needspc ()
{
    char c;
    bool found;
    for (found = false; (c = (char) ch ()) == ' ' || c == '\t'; found = true)
      gch ();
    return found;
}

bool Lexer::spc ()
{
    needspc ();
    return true;
}

int Lexer::ch ()
{
    return _source.ch ();
}

int Lexer::nch (int i)
{
    return _source.nch (i);
}

int Lexer::gch ()
{
    return _source.gch ();
}

int Lexer::mark (int pos)
{
    return _source.mark (pos);
}

bool Lexer::zap (char final)
{
    while (ch () != EOF && ch () != final)
      gch ();
    gch ();
    return false;
}

bool Lexer::ident (string& id)
{
    id.clear ();
    if (! xisalpha (ch ()))
      return false;
    id += (char) gch ();
    while (xisalnum (ch ()))
      id += (char) gch ();
    return true;
}

bool Lexer::xisalpha (int c)
{
    return isalpha (c) || c == '$' || c == '_';
}

bool Lexer::xisalnum (int c)
{
    return isalnum (c) || c == '$' || c == '_';
}

bool Lexer::xisblank (int c)
{
    return c == ' ' || c == '\n' || c == '\t';
}

bool Lexer::number (int& val)
{
    int minus = 1, base;

    if (match ("+"))
      ;
    else if (match ("-"))
      minus = -minus;

    if (match ("0x"))
      base = 16;
    else if (match ("0b"))
      base = 2;
    else if (match ("0"))
      base = 8;
    else
      base = 10;

    // hack : 0 only is considered as octal,  but xnumber fails,
    // because 0 is consumed by base prefix. Anyway, in such a case,
    // xnumber returns val==0, so 0 is considered as 0
    bool r = xnumber (val, base) || base == 8;
    val *= minus;
    return r;
}

/* Parse a number in base 'base'
   '_' is allowed insid a number (but NOT at the beginning)
   and has no effect on the result
   This non standard feature can be used to split numbers into 'blocks'
   a-la ADA (useful improve readability of binary numbers) */
bool Lexer::xnumber (int& val, int base)
{
    int d = 0;

    val = 0;
    // at least one digit is needed
    if (digit (ch (), base) == -1)
      return false;

    for (val = 0; (ch () == '_') || ((d = digit (ch (), base)) != -1); gch ())
      {
	if (ch () != '_')
	  val = (val *base) + d;
      }
    return true;
}

int Lexer::digit (int c, int b)
{
    int r;

    if (c >= '0' && c <= '9')
      r = c - '0';
    else if (c >= 'a' && c <= 'z')
      r = c - 'a' + 10;
    else if (c >= 'A' && c <= 'Z')
      r = c - 'A' + 10;
    else
      r = -1;

    if (r < 0 || r >= b)
      r = -1;
    return r;
}

// Do not use for parsing because of EOF special treatment
string Lexer::nextChars (int n)
{
    int pos = mark ();
    string r;

    for (; n > 0; --n)
      {
	if (ch () == EOF)
	  {
	    r += "<EOF>";
	    break;
	  }
	else if (ch() == '\n')
	  break; // do not cross lines boundaries
	r += (char) gch ();
      }
    mark (pos);
    return r;
}

bool Lexer::kmatch (const char *keyword)
{
    blk ();
    int pos = mark ();
    if (match (keyword) && ! xisalnum (ch ()))
      return true;
    mark (pos);
    return false;
}

// Convert an escape sequence to an unique code
int Lexer::esc_seq ()
{
    int r;

    gch (); /* eat the \ */

    if (ch () == '\\')
      r = gch ();
    else if (ch () == '\'')
      r = gch ();
    else if (ch () == 'n')
      {
	gch ();
	r = '\n';
      }
    else if (ch () == 'r')
      {
	gch ();
	r = '\r';
      }
    else if (ch () == 't')
      {
	gch ();
	r = '\t';
      }
    else if (ch () == '"')
      r = gch ();
    else if (ch () >= '0' && ch () <= '7') // octal constant
      {
	int d;

	r = 0;
	while ((d = digit (ch (), 8)) != -1)
	  {
	    gch ();
	    r = (r << 3) + d;
	  }
	if (r > 127)
	  r -= 256; // TODO: fix signed-unsigned issue
      }
    else if (ch () == 'x') // hex constant
      {
	int d;

	gch ();
	r = 0;
	while ((d = digit (ch (), 16)) != -1)
	  {
	    gch ();
	    r = (r << 4) + d;
	  }
	if (r > 127)
	  r -= 256; // TODO: fix signed-unsigned issue
      }
    else
      r = '\\'; // unrecognized escape sequence

    return r;
}

int Lexer::getLineNum ()
{
    return _source.getLineNum ();
}

// Can be used to pass a fake token to the Lexer
void Lexer::pushBack (const char *s)
{
    _source.pushBack (s);
}

bool Lexer::opMatch (const char *op)
{
    blk ();
    bool r = match (op);
    blk ();
    return r;
}

/* True if s is matched, but not followed by
   any char in r */
bool Lexer::matchReject (const char *s, const char *r)
{
    blk ();
    int i = mark ();
    if (match (s))
      {
	for (; *r; ++r)
	  {
	    if (ch () == *r)
	      {
		mark (i);
		return false;
	      }
	  }
	return true;
      }
    return false;
}
