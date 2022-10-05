/* ========================== 16F54 specific code ====================== */

#include <cstdio>

#include "generic16F54.hh"


generic16F54::generic16F54 ()
{
}

const char *generic16F54::names ()
{
    return "p16f54";
}

const char *generic16F54::SPLocs ()
{
    static char buf[100];
    sprintf (buf, "%X{OSCCAL location}", memaddr2);
    return buf;
}

const char *generic16F54::progOptions ()
{
    return "";
}

void generic16F54::postInstanciate (const char *)
{
    osccal_location = memaddr2;
}
