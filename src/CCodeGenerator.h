/* Simple interface specification for a class which is able to generate C Code 
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef CCODEGENERATOR_H
#define CCODEGENERATOR_H

#include <QString>


class CCodeGenerator
{
public:
    virtual QString makeCode (int selector) = 0;
    virtual ~CCodeGenerator ();
};

#endif
