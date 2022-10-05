/* Represents the complete set of config registers
   for a given device
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef CONFIGBITS_H
#define CONFIGBITS_H

#include <QVector>

#include "CCodeGenerator.h"
#include "ConfigBitsWord.h"
#include "pikdb.hh"


class ConfigBits : public CCodeGenerator
{
public:
    ConfigBits (deviceRecord *deviceData);
    ~ConfigBits ();
    void dump (ostream& os);
    QVector<ConfigBitsWord>& theWords () { return words; }
    friend ostream& operator<< (ostream& out, const ConfigBits& cb);
    const char *setRawValues (const QVector<int>& values);
    QVector<int> getRawValues ();
    void writeCCode (const QString& directory);
    QString makeCCode ();
    void writeASMCode (const QString& directory);
    QString makeASMCode ();
    QString makeCode (int selector);

private:
    QVector<ConfigBitsWord> words;
    deviceRecord *device;
};

#endif
