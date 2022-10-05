/***************************************************************************
                      progressObserver.cc  -  description
                             -------------------
    begin                : Sun Jan 29 2006
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

#include <iostream>
using namespace std;

#include "progressObserver.hh"


progressObserver::progressObserver ()
{
    total_step = 0;
    current_step = 0;
    progress_value = 0;
    client = 0;
}

void progressObserver::setName (const char *name)
{
    process_name = string (" ") + name + " ";
    if (client)
      client->updateProgressText (process_name.c_str ());
}

progressObserver::~progressObserver ()
{
    client = 0;
}

// Used by observed routine to signal termination of a process step.
int progressObserver::jump (int step)
{
    current_step = step;
    int progress = (int) (100.0 * ((double) current_step / total_step));
    if (progress > 100)
      progress = 100;
    if (progress != progress_value)
      {
	progress_value = progress;
	if (client)
	  client->updateProgressBar (progress_value);
      }
    return progress_value;  
}

int progressObserver::operator-- ()
{
    return jump (current_step + 1);
}

int progressObserver::operator-= (int k)
{
    return jump (current_step + k);
}

/* Connect a client progressListener client to progressObserver
   Passing a NULL pointer is equivalent to disconnect observer object from client. */
void progressObserver::connect (progressListener *cli)
{
    client = cli;
}

// Set maximal value of step counter and reset current step counter
int progressObserver::operator= (int total)
{
    total_step = total;
    current_step = 0;
    progress_value = 0;
    if (client)
      client->updateProgressBar (0);
    return total;
}

progressObserver& progressObserver::operator= (const char *text) 
{
    if (client)
      client->updateProgressText (text);
    return *this;
}
