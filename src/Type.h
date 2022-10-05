/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* This is a simplified type representation for definition/declaration
   with minimal semantic */

#ifndef __TYPR_H
#define __TYPR_H

#include <iostream>
#include <iterator>
#include <list>
#include <vector>
using namespace std;

#include "Ctype.h"


class type
{
    int _code; // code of the type (see Code enum)
    string _name; // name of type	
    Ctype _subtype; // link to subtype (ex: array of array)
    string _args; // function parameters list  OR  arraysize
    string _modifiers; // const __interrupt__ etc ..
    unsigned int cnt; // reference counter

    type (); // NO default constructor
    type& operator= (const type&); // NO affectation allowed
    type (const type&); // no clone

    type (int code, const string& name = "", const string& mod = "")
      {
	_code = code;
	_name = name;
	_modifiers = mod;
	cnt = 0;
      }

public:
    friend class Ctype;

    enum Code
      {
	Ptr, // pointer to
	Array,// array of
	Fct, // function returning ..
	Terminal // terminal type (builtin, typedefed or union)
      };

    // this is the only way to allocate a type
    static type *newtype (int code, const string& name = "", const string& mod = "")
      {
	return new type (code, name, mod);
      }

    virtual ~type ();
    type *clone ();

    // Dump function for debugging purposes
    void dump (ostream& os = cout);
    // return the the final type (always a predefined type)
    Ctype& terminaltype ();

    // setters/getters
    string& modifiers () { return _modifiers; } 
    int& code () { return _code; }
    string& args () { return _args; }
    string& name () { return _name; }
    Ctype& subtype () { return _subtype; }

    bool is (int what) { return _code == what; }

    virtual string prettyPrint (const string& decl, const string& name);
};

#endif
