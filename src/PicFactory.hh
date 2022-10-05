/* ----------------------------------------------------------------------- 
   A class to manage the pic collection

   PicFactory is a class to register pic instances, and allows to access these
   instances by name (We use here the same names as in the gpasm assembler
   i.e.: p12c508 for a pic 12C508) .
   For example, a pic12C508 instance will be returned by PicFactory if you need
   to program either a 12c508, 12c508a or 12ce508  flavor of this model.

   Each available pic flavor must be registered in  the PicFactory  by mean of
   the registerPic () function. This is done by default in the PicFactory
   constructor, but may be performed elsewhere.

   As there is really no need to use more that one PicFactory, PicFactory
   implements the singleton pattern. Please use the factory static method
   to obtaint the adresse of the PicFactory

   The following code may be used to register and access a pic instance

   // registration code :
   PicFactory::factory()->registerPic(new pic12C671) ;
   // ...
   // retreive a pic flavor from the registered pic collection
   pic *p =  PicFactory::factory ()->getPic ("p12c671");
   ----------------------------------------------------------------------- */

#ifndef PICFACTORY_H
#define PICFACTORY_H

#include <string>
#include <vector>
using namespace std;

#include "pic.hh"


class PicFactory
{
private:
    vector<pic *> piclist; // internally used to store pic instances
    static PicFactory *the_factory; // internally used to implement singleton pattern

public:
    /* -----------------------------------------------------------------------  
       Access the (single instance of) PicFactory

       Returns:
       The address of the PicFactory
       ----------------------------------------------------------------------- */
    static PicFactory *factory ();    

private:
     // This constructor is private: user must use factory to access the PicFactory
    PicFactory ();
    /* -----------------------------------------------------------------------   
       Search for and id in the id string s

       Returns:
       The index of the id if found, -1 if not found
       ----------------------------------------------------------------------- */
    int findID (const char *s, unsigned int id);

public:
    ~PicFactory ();
    /* -----------------------------------------------------------------------   
       Retreive a pic instance from its gpasm-compatible name

       Parameters:
       "picname" the name of the chip to be programmed

       Returns:
       A pointer to the instance found, or 0 if not found
       ----------------------------------------------------------------------- */
    pic *getPic (const char *picname);
    /* -----------------------------------------------------------------------   
       Get all the PIC names from the collection of the registered pic instances

       Returns:
       A string of blank separated names.
       ----------------------------------------------------------------------- */
    string getPicNames ();
    /* -----------------------------------------------------------------------   
       Register a new pic instance

       Parameters:
       "p" The pic instance. 
       As deallocations are automatically performed by destructor, *p MUST be
       dynamically allocated.

       WARNING: No verification for duplicate registeration is performed.
       ----------------------------------------------------------------------- */
    void registerPic (pic *p);
    /* -----------------------------------------------------------------------   
       Convert a pic id to pic name

       Returns:
       A pic name if found, 
       an empty string if id == 0 or id == ~0 (all bit 0 or all bits 1)
       "?" if the device is not supported
       ----------------------------------------------------------------------- */
    string findDeviceName (unsigned int id);
};

#endif
