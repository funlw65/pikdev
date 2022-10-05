/***************************************************************************
                        generic16F87x.cc  -  description
                             -------------------
    begin                : Sun Jan 22 2006
    copyright            : (C) 2006 by Gibaud Alain
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

#include "generic16F87x.hh"


generic16F87x::generic16F87x ()
{
}

int generic16F87x::erase_pic ()
{
    unsigned int word = 0x3FFF;

    pulseEngine ("cd,P,B,k0,S,k6,k6,k6,k6,k6,k6,k6,k1,k7,k8,w8000,k1,k7,bpcd", &word);
    // this is the sequence for non-protected code
    pulseEngine ("cd,P,B,k2,S,k1,k7,k8,w8000,k1,k7,k3,S,,k1,k7,k8,w8000,k1,k7,b,p", &word);
    return 0;
}

const char *generic16F87x::names ()
{
    return "p16f870 p16f871 p16f872 p16f873 p16f874 p16f876 p16f877";
}
