/***************************************************************************
                       generic16F6xxA.cc  -  description
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

#include "generic16F6xxA.hh"


generic16F6xxA::generic16F6xxA ()
{
}

// Erase : DS141196D page 11
int generic16F6xxA::erase_pic ()
{
    unsigned int dummy ;
    pulseEngine ("cdP,Bw100");
    pulseEngine ("k0,S,w100,k9,w12000cdbp", &dummy); // T_erase 6ms minimum
    return 0;
}

int generic16F6xxA::program_pic (ostream& o)
{
    // this device uses the same algorithms as 16FXX, excepted
    // that command 8 is a "programming only" instead of a
    // "programming erase". So we need to erase the chip before
    // programming it
    erase_pic ();
    return pic16F::program_pic (o);
}

const char *generic16F6xxA::names ()
{
    return "p16f627a p16f628a p16f648a";
}
