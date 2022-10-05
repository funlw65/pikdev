/***************************************************************************
                          ctype.h  -  description
                             -------------------
    begin                : Tue Jan 18 2005
    copyright            : (C) 2005 by Gibaud Alain
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
/* Encapsulates "type" objects and provides reference counting.
   This class avoids type duplication
   @author Gibaud Alain */

#ifndef CTYPE_H
#define CTYPE_H

#include <string>
using namespace std;


class type; // forward

class Ctype
{
    type *thetype ;
public:
    explicit Ctype (type *t = 0);
    Ctype (const Ctype&);
    virtual ~Ctype ();
    Ctype (int code, const string& name = "", const string& modifiers = "");
    // Delegation operator
    type *operator-> ();
    // True if referenced type are the same or identical
    /* used to simulate a comparison to NULL pointer
       result is undefined with a parameter different than 0 */
    bool operator== (int val);
    bool operator!= (int val);
    // Affectation between types with reference counting
    Ctype& operator= (const Ctype&);
    operator bool ();
    type *theType () { return thetype; }
};

#endif
