#include <iostream>
#include <sstream>
using namespace std;

#include "Type.h"


type *type::clone ()
{
    type *p = newtype (_code, _name, _modifiers);
    p->_subtype = _subtype;

    if (is (Fct) || is (Array))
      p->_args = _args;

    return p;
}

// dump a type (debug purpose)
void type::dump (ostream& os)
{
    if (this == nullptr)
      {
	os << "NULL type";
	return;
      }
    os << _modifiers << " ";

    switch (_code)
      {
        case Terminal:
	  os << _name << " ";
	  break;
        case Ptr:
	  os<< "pointer to ";
	  break;
        case Array:
	  os << "array[" << _args << "] of ";
	  break;
        case Fct:
	  os << "function(" << _args << ") returning ";
	  break;
        default:
	  os << "??? (code=" << _code << ")";
	  break;
      }
    if (_subtype)
      _subtype->dump (os);
}

string type::prettyPrint (const string& decl, const string& vname)
{
    if (_code == Terminal)
      return _modifiers + _name + " " + vname + decl;
    else if (_code == Ptr)
      {
	if (_subtype->is (Array) || _subtype->is (Fct))
	  return _subtype->prettyPrint ("(*" + decl + vname + ")", "");
	else
	  return _subtype->prettyPrint ("*" + vname + decl, "");
      }
    else if (is (Array))
      {
	ostringstream s;
	s << vname << "[" << args () << "]";
	return _subtype->prettyPrint (decl + s.str (), "");
      }
    else if (is (Fct))
      {
	string tmp = vname + "(" + args () + ")";
	return _subtype->prettyPrint (decl + tmp, "");
      }

    return "";
}

type::~type ()
{
}

Ctype& type::terminaltype ()
{
  return _subtype ? _subtype->terminaltype () : _subtype;
}
