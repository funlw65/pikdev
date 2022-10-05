/***************************************************************************
                        genericDS39622.hh  -  description
                             -------------------
    begin                : Mon Jan 16 2006
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
/* All devices covered by DS39622 programming specification:
   18F2xx0, 2x21, 2xx5, 4xx0, 4x21, 4xx5
   Current Programming specifications covered : version K
   @author Gibaud Alain */

#ifndef GENERICDS39622_HH
#define GENERICDS39622_HH

#include "generic18Fxx2_xx8.hh"


class genericDS39622 : public generic18Fxx2_xx8
{
public:
    genericDS39622 ();
    virtual const char *names ();
    virtual int erase_pic ();
    virtual int program_pic (ostream& out);
    virtual void read_pic ();
};

#endif
