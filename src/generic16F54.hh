#ifndef PIC16F54_HH
#define PIC16F54_HH

#include "generic10F20x_12F50x.hh"


class generic16F54 : public generic10F20x_12F50x
{
public:
    generic16F54 ();
    virtual const char *names ();
    virtual const char *SPLocs ();
    virtual const char *progOptions ();
    virtual void postInstanciate (const char *);
};

#endif
