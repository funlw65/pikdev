/***************************************************************************
                       generic16C8x.cpp  -  description
                             -------------------
    begin                : Tue Jan 24 2006
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

#include "generic16C8x.hh"


generic16C8x::generic16C8x ()
{
}

int generic16C8x::erase_pic ()
{
    return 16; // NA
}

const char *generic16C8x::names ()
{
    return "p16cr83 p16cr84";
}
