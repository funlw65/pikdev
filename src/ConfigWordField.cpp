#include <cstdio>

#include "ConfigWordField.h"


ConfigWordField::ConfigWordField ()
{
    value_index = -1;
}

ConfigWordField::~ConfigWordField ()
{
}

bool ConfigWordField::parse (const char*& p)
{
    char bitname[60];
    int a, b, n, i;
    const char *msg;

    // reset field descriptor
    field_name = "";
    field_description = "";
    autonum = false;
    field_values.clear ();

    // skip blanks
    while (*p ==  ' ')
      ++p;
    // XX[a-b] means XX0 ... XXa-b+1
    i = sscanf (p, "%[_$|A-Za-z0-9/-][%d-%d]%n", bitname, &a, &b, &n);
    if (i == 3)
      {
	autonum = true;
	field_name = bitname;
	if (a <= b)
	  min = a, max = b;
	else
	  min = b, max = a;
	p += n;
	msg = parseFieldValues (p);
	if (msg)
	  cout << msg << endl;
	makeDefaultValue ();
	return true;
      }
    // XX(a-b) means that all the bits from b to a are identical
    // and are (identically) named XX
    i = sscanf (p, "%[_$|A-Za-z0-9/-](%d-%d)%n", bitname, &a, &b, &n);
    if (i == 3)
      {
	field_name = bitname;
	if (a <= b)
	  min = a, max = b;
	else
	  min = b, max = a;
	p += n;
	msg = parseFieldValues (p);
	if (msg)
	  cout << msg << endl;
	makeDefaultValue ();
	return true;
      }
    // try XX[a]
    i = sscanf (p, "%[_$|A-Za-z0-9/-][%d]%n", bitname, &a, &n);
    if (i == 2)
      {
	field_name = bitname;
	min = max = a;
	p += n;
	msg = parseFieldValues (p);
	if (msg)
	  cout << msg << endl;
	makeDefaultValue ();
	return true;
      }
    // XX[a] and XX(a) have the same meaning
    i = sscanf (p, "%[_$|A-Za-z0-9/-](%d)%n", bitname, &a, &n);
    if (i == 2)
      {
	field_name = bitname;
	min = max = a;
	p += n;
	msg = parseFieldValues (p);
	if (msg)
	  cout << msg << endl;
	makeDefaultValue ();
	return true;
      }
    return false;
}

/* ----------------------------------------------------------------------- 
   Build the field value descriptor

   Returns:
   0 if OK,
   else pointer on error message
   ----------------------------------------------------------------------- */
const char *ConfigWordField::parseFieldValues (const char*& p)
{
    field_values.clear ();

    while (*p == ' ')
      ++p;

    if (*p == '{')
      {
	FieldValue entry;
	QString& valname = entry.description;
	QString& symbolicValname = entry.symbolicValue;
	int& val = entry.value;

	// parse short description of field
	for (++p; *p && *p != '\n' && *p != '}'; ++p)
	  field_description += *p;

	if (*p == '\n')
	  ++p;

	// parse allowed values (binary value=text)
	while (*p != '}')
	  {
	    val = 0;
	    valname = "";

	    // parse  binary values
	    for (; *p == '1' || *p == '0'; ++p)
	      {
		val <<= 1;
		if (*p == '1')
		  val |= 1;
	      }

	    // ++Gib: 2013 TODO parse symboic name of field-value,
	    // ex: 101@XXX=description
	    symbolicValname.clear ();
	    if (*p == '@')
	      {
		for (++p; *p && *p != '=' && *p != '\n'; ++p)
		  symbolicValname += *p;
	      }

	    if (*p != '=')
	      return "Malformed field value descriptor ('=' symbol expected)";

	    // parse text
	    for (++p; *p && *p != '}' && *p != '\n'; ++p)
	      valname += *p;

	    if (valname == "")
	      return "Field value has no name";
	    if (*p == '\n')
	      ++p;

	    field_values.push_back (entry);
	  }
	++p;
	return 0;
      }

    return 0;
}

#if 0
const char *ConfigWordField::parseFieldValues (const char*& p)
{
    field_values.clear ();

    while (*p == ' ')
      ++p;

    if (*p == '{')
      {
	QPair<int, QString> entry;
	QString& valname = entry.second;
	int& val = entry.first;

	// parse short description of field
	for (++p; *p && *p != '\n' && *p != '}'; ++p)
	  field_description += *p;

	if (*p == '\n')
	  ++p;

	// parse allowed values (binary value=text)
	while (*p != '}')
	  {
	    val = 0;
	    valname = "";

	    // parse  binary values
	    for (; *p == '1' || *p == '0'; ++p)
	      {
		val <<= 1;
		if (*p == '1')
		  val |= 1;
	      }

	    // ++Gib: 2013 TODO parse symboic name of field-value,

	    if (*p != '=')
	      return "Malformed field value descriptor ('=' symbol expected)";

	    // parse text
	    for (++p; *p && *p != '}' && *p != '\n'; ++p)
	      valname += *p;
	    if (valname == "")
	      return "Field value has no name";
	    if (*p == '\n')
	      ++p;

	    field_values.push_back (entry);
	  }
	++p;
	return 0;
      }

    return 0;
}

#endif

void ConfigWordField::dump (ostream& os)
{
    os << "name=" << field_name.toStdString () << endl;
    os << "description=" << field_description.toStdString () << endl;
    if (field_values.size ())
      {
	os << "bits [" << min << "-" << max << "]";
	os << " with possible values :" << endl;
	for (int i = 0; i < field_values.size (); ++i)
	  {
	    os << field_values[i].value << "="
	       << field_values[i].description.toStdString () << endl;
	  }
	os << "autonum : " << (autonum ? "yes" : "no") << endl;
      }
}

/* Make the default value for this field
   The default value is:
   -All 0 if the name of the field name is "0"
   -All 1 if the name of the field name is "1"
   -The first legal value otherwise

   The value is correctly left shifted so the final config word value
   can be computed by simply ORing all fields values. */
void ConfigWordField::makeDefaultValue ()
{
    value_index = -1;

    if (field_name == "0")
      value = 0;
    else if (field_name == "1")
      value = (1 << (max - min + 1)) - 1;
    else if (field_values.size () != 0)
      value = field_values[value_index = 0].value;

    // put the bit field at the correct location
    value <<= min;
}

int ConfigWordField::getValueIndex ()
{
    return value_index;
}

/* Set field to Nth legal value
   (first legal value corresponds to index 0) */
bool ConfigWordField::setValue (int N)
{
    bool r = false;
    value = 0;
    if (N >= 0 && N < (int) field_values.size ())
      {
	value = field_values[N].value << min;
	value_index = N;
	r = true;
      }
    return r;
}

// Return the number of legal values for this field
int ConfigWordField::legalValuesCount ()
{
    return field_values.size ();
}

/* Return the description of the legal value N
   ++Gib: 2013
   When a symbolic value existe (PIC18F), it is added before each description */
QString ConfigWordField::legalValueDesc (int N)
{
    if (N >= 0 && N < (int) field_values.size ())
      {
        QString desc = field_values[N].symbolicValue;
        if (desc.size () != 0)
	  desc += "  -  ";
        desc += field_values[N].description;
        return desc;
      }
    return "?";
}

// Return the legal value N
int ConfigWordField::legalValue (int N)
{
    if (N >= 0 && N < (int) field_values.size ())
      return field_values[N].value;

    return 0;
}

// Return "sized" name, such as XXX [n-m]
QString ConfigWordField::nameAndSize ()
{
    QString r = field_name;

    if (min != max)
      {
	r += "[0-%1] ";
	r = r.arg (max - min);
      }
    return r;
}

// Extract field value from word & initialize the field
bool ConfigWordField::makeFieldFromWord (int word)
{
    // get field
    word >>= min;
    word &= ((1 << (max - min + 1)) - 1);

    // get corresponding legal value
    for (int i = 0; i < legalValuesCount (); ++i)
      {
	if (legalValue (i) == word)
	  {
	    setValue (i);
	    return true;
	  }
      }
    makeDefaultValue ();
    return false;
}

/* Make a string describing the current state of this field,
   for auto-documentation of generated code */
QString ConfigWordField::humanReadable ()
{
    QString r = nameAndSize ();

    r += " (";
    r += field_description;
    r += ") = ";
    r += binaryString ();
    r += " (";
    r += legalValueDesc (getValueIndex ());
    r += ")";

    return r;
}

// Make a binary string from field value
QString ConfigWordField::binaryString ()
{
    QString r;
    
    for (int i = max; i >= min; --i)
      r += ((1 << i) & value) ? '1' : '0';
    return r;
}

/* Return a mask which allows to locate used bits
   (a set bit int the mask corresponds to a used bit in the field) */
int ConfigWordField::mask ()
{
    int r = 0;

    for (int i = max; field_description != "Unused" && i >= min; --i)
      r |= (1 << i);
    return r;
}

/* Return the symbolic form of the field (i.e.: X = Y)
   where X is the field name, and Y the symbolic name of the value (eg: ON or OFF)
   Return an empty string for non PIC18 devices, or when no current value is selected */
QString ConfigWordField::symbolicString ()
{
    if (value_index != -1)
      {
        QString fieldValue = field_values[value_index].symbolicValue;
        if (fieldValue == "")
	  return "";
        return field_name + "=" + fieldValue;
      }
    return "";
}
