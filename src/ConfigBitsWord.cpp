#include "ConfigBitsWord.h"
#include "ConfigWordField.h"


ConfigBitsWord::ConfigBitsWord (const QString& name, int addr, const QString& desc)
{
    /* passing through a local QByteArray is essential
       because the pointer is used after the destruction of the
       temporary QByteArray */
    QByteArray pp = desc.toLatin1 ();
    const char* p = pp.data ();
    ConfigWordField f;
    word_addr = addr;
    word_name = name;

    for (; f.parse (p); )
      fields.push_back (f);
}

void ConfigBitsWord::dump (ostream& os)
{
    os << word_name.toStdString ();
    os << hex << " ADDR=" << word_addr << endl;

    for (int i = 0; i < fields.size (); ++i)
      {
        fields[i].dump (os);
	os << endl;
      }
}

ConfigBitsWord::~ConfigBitsWord ()
{
}

int ConfigBitsWord::value () const
{
    int v = 0;
    
    for (int i = 0; i < fields.size (); ++i)
      v |= fields[i].getValue ();
    return v;
}

void ConfigBitsWord::setValue (int v)
{
    for (int i = 0; i < fields.size (); ++i)
      fields[i].makeFieldFromWord (v);
}

QString ConfigBitsWord::humanReadable (const QString& newline)
{
    QString r = name ();
    r += " at 0x";
    r += QString::number (addr (), 16);
    r += " value = 0x";
    r += QString::number (value (), 16);
    r += " - 0b";
    r += binaryString ();
    r += newline;
    r += "---------------------";
    r += newline;

    for (int i = 0; i < (int) theFields ().size (); ++i)
      {
	r += theFields ()[i].humanReadable ();
	r += newline;
      }
    return r;
}

int ConfigBitsWord::mask ()
{
    int v = 0;
    
    for (int i = 0; i < fields.size (); ++i)
      v |= fields[i].mask ();
    return v;
}

QString ConfigBitsWord::binaryString ()
{
    QString r, sep;

    for (int i = 0; i < fields.size (); ++i)
      {
	r += sep; sep = "_";
	r += fields[i].binaryString ();
      }
    return r;
}

QString ConfigBitsWord::hexString ()
{
    return QString::asprintf ("0x%04X", value ());
}

QString ConfigBitsWord::symbolicString ()
{
    QString r;
    
    for (int i = 0; i < fields.size (); ++i)
      {
	QString f = fields[i].symbolicString ();
	if (f != "")
	  {
	    if (r != "")
	      r += ",";
	    r += f;
	  }
      }
    return r;
}
