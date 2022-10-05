/***************************************************************************
                        fragBuffer.h  -  description
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
/* This class implements a fragmented buffer.
   (i.e.: large buffer with many unused blocks).
   Its main purpose is to manage inhx32 hex files used for PIC18xxxx devices.
   @author Alain Gibaud */

#ifndef FRAGBUFFER_H
#define FRAGBUFFER_H

#include <map>


// an helper class  
class FragBuffer : public std::map<unsigned int,unsigned int>
{
public: 
    FragBuffer ();
    ~FragBuffer ();
    void setBlock (unsigned int start, unsigned int stop, unsigned int val = 0xFFFFFFFFU);
    unsigned int& operator* ();  // emulates a *buf (== buf[0]) operation
    // insure that a never used word is read as 0xFFFFFFFFU
    unsigned int& operator[] (unsigned int);
    unsigned int *operator+ (unsigned int); // emulates a pointer + integer operation
    bool exists (unsigned int k); // true is the elements k exists
    bool valid (unsigned int k); // true if the element k exits and is not 0xFFFFFFFFU
};

#endif
