/* Base class for all class which need to gain access to console 
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef CONSOLECLIENT_H
#define CONSOLECLIENT_H

#include <QString>


class ConsoleClient 
{
public:
    ConsoleClient ();
    ~ConsoleClient ();
    void clearConsole ();
    void addTextConsole (const QString& txt);
};

#endif
