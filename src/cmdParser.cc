#include <iostream>
using namespace std;

#include "cmdParser.hh"


cmdParser::cmdParser ()
{
}

string cmdParser::analyse (int argc, char *argv[])
{
    string r;
    opts.clear ();
    params.clear ();

    for (int i = 0; r == "" && i < argc; ++i)
      {
        const char *a = argv[i] ;
	if (a[0] == '-' && a[1] == '-')
          r = parseOption (a);
	else
	  params.push_back (string (a));
      }
    return r;
}

bool cmdParser::hasOption (const string& opt) 
{
    return opts.find (opt) != opts.end ();
}

string cmdParser::optValue (const string& key) 
{
    string r;
    
    if (hasOption (key))
      r = opts[key];
    return r;
}

int cmdParser::nbOptions ()
{
    return opts.size ();	
}

int cmdParser::nbParams () 	
{
    return params.size ();	
}

/* parse long option (i.e.: --key = parameter) */
string cmdParser::parseOption (const char *p) 
{
    string key, value;

    while (*p == ' ')
      ++p;
    
    if (p[0] == '-' && p[1] == '-')
      {
	for (p += 2; *p && *p != '='; ++p)
	  key += *p;

	if ((optionData (key) & Exist) == 0)
	  return "Invalid option : --" + key;

	bool ok = (*p == '=' && (optionData (key) & HasParameter))
	          || (*p != '=' && (optionData (key) & HasNoParameter));

	if (! ok)
	  {
	    if ((optionData (key) & HasParameter))
	      return "Missing parameter for option : --" + key;
	    else
	      return "This option doesn't accept parameter : --" + key;
	  }	

	if (*p)
	  ++p; // skip '='

	for (; *p; ++p)
	  value += *p;
	
	if (hasOption (key))
	  return "Option used twice : --" + key;
	
	opts[key] = value;
      }
    return "";	
}

string cmdParser::param (int k)
{
    string r;
    
    if (k >= 0 && k < nbParams())
      r = params[k];
    return r;
}

cmdParser::optIterator cmdParser::optBegin ()
{
    return opts.begin ();
}

cmdParser::optIterator cmdParser::optEnd ()
{
    return opts.end ();
}
	
void cmdParser::addOption (const string& optname, int type) 
{
    validopts[optname] = type | cmdParser::Exist;
}

int cmdParser::optionData (const string& opt) 
{
    map<string, int>::iterator i = validopts.find (opt);
    return i != validopts.end () ? (*i).second : None;
}
