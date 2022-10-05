/***************************************************************************
                       ProgrammerBase.cpp  -  description
                             -------------------
    begin                : Tue Jan 31 2006
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

#include "ProgrammerBase.h"


ProgrammerBase::ProgrammerBase () : thepic (0)
{  
}

ProgrammerBase::~ProgrammerBase ()
{
}

pic *ProgrammerBase::device ()
{
    return thepic;
}

QString ProgrammerBase::extraFct () 
{
    return "";
}

bool ProgrammerBase::executeExtraFct (__attribute__ ((unused)) int function_number)
{
    return true;
}
