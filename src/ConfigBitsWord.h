/* Config word representation, up to 32 bits
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef CONFIGBITSWORD_H
#define CONFIGBITSWORD_H

#include <iostream>
using namespace std;

#include "ConfigWordField.h"


class ConfigBitsWord
{
public:
    ConfigBitsWord (const QString& name, int addr, const QString& desc);
    ConfigBitsWord () {}
    ~ConfigBitsWord ();
    void dump (ostream& os);
    QVector<ConfigWordField>& theFields () { return fields; }
    QString name () { return word_name; }
    int addr () { return word_addr; }
    int value () const;
    void setValue (int);
    QString humanReadable (const QString& newline);
    int mask ();
    QString binaryString ();
    QString hexString ();
    QString symbolicString ();
  
private:
    QString word_name; // CONFIG or CONFIG1H etc.
    int word_addr; // addr of this word
    QVector<ConfigWordField> fields; // fields representation
};

#endif
