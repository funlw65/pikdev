/* Interface for programmers's clients displaying a progress bar (or equivalent)
   @author Gibaud Alain <alain.gibaud@free.fr> */

#ifndef PROGRESSLISTENER_HH
#define PROGRESSLISTENER_HH


class progressListener
{
public:
    virtual void updateProgressBar (int value) = 0;
    virtual void updateProgressText (const char *text) = 0;
    virtual ~progressListener ();
};

#endif
