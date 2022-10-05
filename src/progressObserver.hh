/***************************************************************************
                      progressObserver.hh  -  description
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
/* Helper class which can be used by programming code to notify progress status.
   @author Gibaud Alain */

#ifndef PROGRESSOBSERVER_HH
#define PROGRESSOBSERVER_HH

#include <string>
using namespace std;

#include "progressListener.hh"


class progressObserver
{
    progressListener *client; // the client object
    int total_step; // total step to be performed (see operator= and operator++)
    int current_step; // current step
    int progress_value; // current percentage, computed from current_step
    string process_name; // The observed process name (not used for now)

public:
    progressObserver ();
    ~progressObserver ();

    /* ----------------------------------------------------------------------- 
       Connect a client to observer.

       The valu passed to updateProgressBar() method is normalized in [0..100].
       Passing a NULL pointer is equivalent to disconnect observer object from
       client.
       ----------------------------------------------------------------------- */
    void connect (progressListener *c);

    /* ----------------------------------------------------------------------- 
       Signal step achievement

       Used by observed routine to signal termination of the current step to
       client.
       operator-- calls the function specified by connect when the resulting
       progress percentage changes.

       Returns:
       The remaining step count
       ----------------------------------------------------------------------- */
    int operator-- ();
  
    /* -----------------------------------------------------------------------  
       Signal several step achievement

       Used by observed routine to signal termination of k steps to client.
       operator-- calls the function specified by connect when the resulting
       progress percentage changes.

       Returns:
       The current progress percentage
       ----------------------------------------------------------------------- */
    int operator-= (int k);

    /* -----------------------------------------------------------------------  
       Attach an identifier to progressObserver instance
    
       Note:
       Not yet used
       ----------------------------------------------------------------------- */
    void setName (const char *identifier);

    /* -----------------------------------------------------------------------  
       Set initial value of step counter

       Returns:
       its argument totalstep
       ----------------------------------------------------------------------- */
    int operator= (int totalstep);
  
    /* -----------------------------------------------------------------------  
       Jump to specified step (0 <= step <= total_step)

       Returns:
       The current progress percentage
       ----------------------------------------------------------------------- */
    int jump (int step);
  
    /* -----------------------------------------------------------------------
       Change text about the current step

       Returns:
       this
       ----------------------------------------------------------------------- */
    progressObserver& operator= (const char *text);
};

#endif
