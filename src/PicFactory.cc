#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "PicFactory.hh"
#include "generic10F20x_12F50x.hh"
#include "generic12C5x8_5x9.hh"
#include "generic12C67x_CE67x.hh"
#include "generic12F629_675.hh"
#include "generic12_16F6xx.hh"
#include "generic16C6xx_7xx_9xx.hh"
#include "generic16C8x.hh"
#include "generic16F54.hh"
#include "generic16F627_628.hh"
#include "generic16F6xxA.hh"
#include "generic16F7x.hh"
#include "generic16F818_819.hh"
#include "generic16F83_84.hh"
#include "generic16F87x.hh"
#include "generic16F87xA.hh"
#include "generic18F6x2x_8x2x.hh"
#include "generic18Fxx2_xx8.hh"
#include "genericDS39622.hh"  // many 18F
// #include "generic12F609_615.hh"
// add here headers for new pic flavors


PicFactory *PicFactory::the_factory = 0;

PicFactory *PicFactory::factory ()
{
    return (the_factory == 0) ? (the_factory = new PicFactory) : the_factory;
}

// This method must be updated when a new pic driver is developed
PicFactory::PicFactory ()
{
    // 12C family
    registerPic (new generic12C67x_CE67x);
    registerPic (new generic12C5x8_5x9);
    // 16F family
    registerPic (new generic16F54);
    registerPic (new generic16F627_628);
    registerPic (new generic16F6xxA);
    registerPic (new generic16F7x);
    registerPic (new generic16F818_819);    
    registerPic (new generic16F87x);
    registerPic (new generic16F87xA);
    registerPic (new generic16F83_84);    
    // 16C family
    registerPic (new generic16C8x);
    registerPic (new generic16C6xx_7xx_9xx);    
    // 18F family
    registerPic (new generic18F6x2x_8x2x);
    registerPic (new genericDS39622);
    registerPic (new generic18Fxx2_xx8);
    // 12F 14 bit family (12F6xx are in fact 14 bit pics)
    registerPic (new generic12F629_675);
    //registerPic (new generic12F609_615);
    registerPic (new generic12_16F6xx);    
    // 10F & 12F 12 bit family
    registerPic(new generic10F20x_12F50x);    
}

PicFactory::~PicFactory ()
{
    vector<pic *>::iterator i;

    for (i = piclist.begin (); i != piclist.end (); ++i)
      delete *i;
}

// returns a pic flavor, or NULL if not found
#if 1
pic *PicFactory::getPic (const char *picname)
{
    vector<pic *>::iterator i;

    for (i = piclist.begin (); i != piclist.end (); ++i)
      {
        string nn ((*i)->names ());
        istringstream st (nn);
        string n;
        while (st >> n)
	  {
            if (n == picname)
	      {
                (*i)->instanciate (picname); 
                return (*i);
	      }
	  }
      }
    return 0; // not found 
}
#else

pic *PicFactory::getPic (const char *picname)
{
    vector<pic *>::iterator i;
    cout << "piclist size = " << piclist.size () << endl << flush;
    cout << "pic to find = " << picname << endl << flush;

    for (i = piclist.begin (); i != piclist.end (); ++i)
      {
        string nn ((*i)->names ());

	cout << "nn=[" << nn << "]" << endl << flush;
        istringstream st (nn); // I suppose the crash is here
	cout << "st constructor passed " << endl << flush;

        string n;
	cout << "st state = " << st << " " << st.rdstate() << endl << flush;
	if (st.rdstate () & ifstream::failbit)
	  cout << "failbit is on " << endl << flush;
	if (st.rdstate () & ifstream::badbit)
	  cout << "badbit is on " << endl << flush;
	if (st.rdstate () & ifstream::eofbit)
	  cout << "eofbit is on " << endl << flush;

        while (st >> n)
	  {
	    cout << "decoded device = " << n << endl << flush;
            if (n == picname)
	      {
		cout << "before instanciate" << endl << flush;
		(*i)->instanciate (picname); 
 		cout << "after instanciate" << endl << flush;
		return (*i);
	      }
	  }
      }
    return 0; // not found
}
#endif

// May be used to add a new pic flavor after the construction
// of the pic factory
void PicFactory::registerPic (pic *p)
{
    piclist.push_back (p);
}

// Returns a blank-separated list of all pic names
string PicFactory::getPicNames ()
{
    vector<pic *>::iterator i;
    string s;

    for (i = piclist.begin (); i != piclist.end (); ++i)
      {
        s += (*i)->names ();
        s += " ";
      }
    return s;
}

/* ----------------------------------------------------------------------- 
   Convert a pic id to pic name

   Returns:
   A pic name if found, 
   an empty string if id == 0 or id == All one
   "?" if the device is not supported
   ----------------------------------------------------------------------- */
string PicFactory::findDeviceName (unsigned int id)
{
    if (id == 0 || id == 0x3FE0 || id == 0xFFE0)
      return "";

    vector<pic *>::iterator i;

    for (i = piclist.begin (); i != piclist.end (); ++i)
      {
        int idnum = findID ((*i)->IDs (), id);
        if (idnum != -1)
	  {
            string nn ((*i)->names ());
            istringstream st (nn);
            string n;
            for(; idnum >= 0 && st >> n; --idnum)
	      ;
            return n;
	  }
      }
    return "?";
}

/* ----------------------------------------------------------------------- 
   Search for and id in the id string s

   Returns:
   The index of the id if found, -1 if not found
   ----------------------------------------------------------------------- */
int PicFactory::findID (const char *s, unsigned int id)
{
    int n , k ;
    unsigned int i ;
    for (k = 0; sscanf (s, "%x%n", &i, &n) == 1; s += n, ++k)
      {
        if (i == id)
	  return k;
      }
    return -1 ;
}
