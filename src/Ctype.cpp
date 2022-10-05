/***************************************************************************
                          ctype.cpp  -  description
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

#include <assert.h>

#include "Ctype.h"
#include "Type.h"


Ctype::Ctype (const Ctype& ct)
{
    thetype = ct.thetype ;
    if (thetype)
      ++(thetype->cnt);
}

Ctype::~Ctype ()
{
    if (thetype && -- (thetype->cnt) == 0)
      {
	delete thetype;
	thetype = 0;
      }
}

type *Ctype::operator-> ()
{
    return thetype;
}

Ctype::Ctype (int code, const string& name, const string& modifiers)
{
    thetype = type::newtype (code, name, modifiers);
    ++thetype->cnt;
}

bool Ctype::operator== (int val)
{
    assert (val == 0);
    return thetype == reinterpret_cast<type*> (val);
}

bool Ctype::operator!= (int val)
{
    return ! (*this == val);
}

Ctype& Ctype::operator= (const Ctype& ct)
{
    if (this != &ct)
      {
	if (thetype && -- (thetype->cnt) == 0)
	  delete thetype;

	if (ct.thetype)
	  ++(ct.thetype->cnt);

	thetype = ct.thetype;
      }
    return *this;
}

Ctype::operator bool ()
{
    return thetype != 0;
}

Ctype::Ctype (type *t)
{
    thetype = t;
    if (t)
      ++(t->cnt);
}
