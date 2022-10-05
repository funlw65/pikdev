/***************************************************************************
                        fragBuffer.cc  -  description
                             -------------------
    begin                : Fri Jun 13 2003
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
using namespace std;

#include "FragBuffer.hh"


FragBuffer::FragBuffer ()
{
}

FragBuffer::~FragBuffer ()
{
}

void FragBuffer::setBlock (unsigned int start, unsigned int stop, unsigned int val)
{
    for (unsigned int i = start; i <= stop; ++i)
      (*this)[i] = val;
}

unsigned int& FragBuffer::operator* ()
{
    return (*this)[0];
}

unsigned int *FragBuffer::operator+ (unsigned int index)
{
    return &((*this)[index]);	
}

// true if the element k exists
bool FragBuffer::exists (unsigned int k)
{
    return find (k) != end ();
}

// true if the element k exists and does not contain 0xFFFFFFFFU
bool FragBuffer::valid (unsigned int k)
{
    return exists (k) && (*this)[k] != 0xFFFFFFFFU;
}

unsigned int& FragBuffer::operator[] (unsigned int k)
{
    FragBuffer::iterator i = find (k);
    if (i != end ())
      return (*i).second;
    std::map<unsigned int, unsigned int>::operator[](k) = 0xFFFFFFFFU;
    return std::map<unsigned int, unsigned int>::operator[](k);
}
