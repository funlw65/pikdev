/* Helper class for ConfigBitsWord :
   Field representation
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef CONFIGWORDFIELD_H
#define CONFIGWORDFIELD_H

#include <iostream>
using namespace std;

#include <QPair>
#include <QString>
#include <QVector>


class FieldValue
{
public:
    int value; // numeric value
    QString symbolicValue; // symbolic equivalent of value
    QString description; // meaning of this value
};

class ConfigWordField
{
public:
    ConfigWordField ();
    ~ConfigWordField ();

    bool parse (const char*& p);
    const char *parseFieldValues (const char*& p);
    void makeDefaultValue ();

    bool setValue (int N); // set this field to legal value #N (return false if N incorrect)
    int getValueIndex (); // return N
    int legalValuesCount (); // number of legal values
    QString legalValueDesc (int N); // return description of legal value #N
    int legalValue (int N); // return value of legal value #N

    QString name () { return field_name; }
    QString nameAndSize ();
    QString description () { return field_description; }
    int getValue () const { return value; }
    int getMini () const { return min; }
    QString humanReadable ();
    QString binaryString ();
    /* return the symbolic form of the field (i.e.: X = Y)
       where X is the field name, and Y the symbolic name of the value (eg: ON or OFF)
       Return an empty string for non PIC18 devices, or when no current value is selected */
    QString symbolicString ();

    bool makeFieldFromWord (int word);

    QVector<FieldValue>& fieldValues () { return field_values; }

    void dump (ostream& os);
    int mask ();

private:
    QString field_name, // official name from microchip doc
            field_description; // shord description of the field
    int min, max; // field goes from bit number min, to bit number max (limits included)  
    /* true if a bit number should be automatically appended to field name
       (i.e. OSC-0, OSC-1 etc from OSC) */
    bool autonum;
    // all legal values (value + description + symbolic name, if any)
    QVector<FieldValue> field_values;
    int value; // current field value
    int value_index;
};

#endif
