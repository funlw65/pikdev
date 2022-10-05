#include <iomanip>
#include <iostream>
using namespace std;

#include <QDateTime>
#include <QFile>
#include <QString>
#include <QTextStream>

#include "ConfigBits.h"


ConfigBits::ConfigBits (deviceRecord *deviceData) : device (deviceData)
{
    int cf1, cf2;
    QString config = device->configDesc; // description of config registers

    // get pic info
    cf1 = device->cfaddr1;
    cf2 = device->cfaddr2;

    // extract each word description
    for (int i = cf1, k = 0, confnum = 1; i <= cf2; ++i, ++k, ++confnum)
      {
	QString s = config.section (";", k, k);
	if (! s.isNull () && s != "")
	{
	  int adr = i;
	  QString confname = "CONFIG";
	  if (device->core == 16)
	    {
	      // compute config register name
	      QString confname = QString::asprintf ("CONFIG%dHL", confnum);
	      adr *= 2;
	    }
	  ConfigBitsWord w (confname, adr, s);
	  words.push_back (w);
	}
      }
}

/* This is a debug fct */
void ConfigBits::dump (ostream& os)
{
    for (int i = 0; i < words.size (); ++i)
      {
        words[i].dump (os);
	os << endl;
      }
}

ConfigBits::~ConfigBits ()
{
}

ostream& operator<< (ostream& out, const ConfigBits& cb)
{
    for (int i = 0; i < cb.words.size (); ++i)
      out << "0x" << hex << cb.words[i].value () << " ";
    return out;
}

const char *ConfigBits::setRawValues (const QVector<int>& values)
{
    if (values.size () == 0)
      return "Config bits set to default values";
    else if (values.size () != words.size ())
      return "Number of config words does not match the current device: values ignored";
    else
      {
	for (int i = 0; i < words.size (); ++i)
	  words[i].setValue (values[i]);
      }
    return 0;
}

QVector<int> ConfigBits::getRawValues ()
{
    QVector<int> r;

    for (int i = 0; i < words.size (); ++i)
      r.push_back (words[i].value ());
    return r;
}

// Write cpik code for config bits settings
void ConfigBits::writeCCode (const QString& directory)
{
    QString fname = directory + "/config_bits.h";
    QString b;

    QFile file (fname);
    if (file.open (QIODevice::WriteOnly))
      {
	QTextStream stream (&file);
	stream << makeCCode ();
      }
}

QString ConfigBits::makeCCode ()
{
    QString txt;
    QString b;

    QTextStream stream (&txt, QIODevice::WriteOnly);
    QDateTime today = QDateTime::currentDateTime ();
    QString devname = device->gpName;
    stream << "/*\n";
    stream << "This code has been automatically generated and might be overwritten - edit at your own risk !\n";
    stream << "Device : " << devname << "\n";
    stream << "Date : " << today.toString () << "\n";
    stream << "Author : pikdev config word generator\n";
    stream << "*/\n";
    for (int i = 0; i < (int) theWords ().size (); ++i)
      {
	stream << "\n/*\n";
	stream << theWords ()[i].humanReadable ("\n");
	stream << "*/\n";
	int v = theWords ()[i].value ();
	if (device->core == 16)
	  {
	    QString symb = theWords ()[i].symbolicString ();
	    if (symb != "")
	      {
		b = "#pragma config " + symb;
		stream << b << Qt::endl;
	      }
	  }
	else
	  stream << QString::asprintf ("#pragma _CONFIG 0x%0X", v) << Qt::endl;
      }
    return txt;
}

// Write cpik code for config bits settings
void ConfigBits::writeASMCode (const QString& directory)
{
    QString fname = directory + "/config_bits.inc";
    QString b;

    QFile file (fname);
    if (file.open (QIODevice::WriteOnly))
      {
	QTextStream stream (&file);
	stream << makeASMCode ();
      }
}

// Make cpik code for config bits settings
QString ConfigBits::makeASMCode ()
{
    QString b, txt;

    QTextStream stream (&txt, QIODevice::WriteOnly);
    QDateTime today = QDateTime::currentDateTime ();
    QString devname = device->gpName;
    stream << "; \n";
    stream << "; This code has been automatically generated, and might be overwritten  - edit at your own risk !\n";
    stream << "; Device : " << devname << "\n";
    stream << "; Date : "<< today.toString () << "\n";
    stream << "; Author : pikdev config word generator\n";
    stream << "; \n";
    for (int i = 0; i < (int) theWords ().size (); ++i)
      {
	stream << "\n\n; ";
	stream << theWords ()[i].humanReadable ("\n; ");
	stream << "\n";
	int v = theWords ()[i].value ();
	if (device->core == 16)
	  {
	    // PIC18 specific config syntax
	    QString symb = theWords ()[i].symbolicString ();
	    if (symb != "")
	      {
		b = "\tCONFIG " + symb;
		stream << b << Qt::endl;
	      }
	  }
	else
	  stream << QString::asprintf ("\t__CONFIG  0x%0X", v) << Qt::endl;
      }
    return txt;
}

QString ConfigBits::makeCode (int selector)
{
    switch(selector)
      {
	case 1:
	  return makeCCode ();
	case 2:
	  return makeASMCode ();
      }
    return "ConfigBits::makeCode () : bad code selector";
}
		
