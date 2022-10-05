/*  A.GIBAUD 2009-06-21

    This is a simple C parser for parsing
    global definition/declaration.

    Issue: Since preprocessor directives are ignored
    code sections disabled by a preprocessor directive
    are always processed. */

#include <iostream>
#include <sstream>
using namespace std;

#include "Type.h"
#include "parser.h"


Parser::Parser () : stream (0)
{
}

Parser::~Parser ()
{
    if (stream)
      delete stream;
}

/* return :
   0 = OK
   1 = can't connect
   2 = parse error(s) THIS DIAGNOSTIC IS CURRENTLY DISABLED */
int Parser::parse (QBEditorDocument *doc)
{
    errCount = 0;

    if (doc == 0)
      return 1;

    symbTable.clear ();

    file = doc->fileFullName ().toStdString ();

    lx.connectDocument (doc);

    while (blk (), lx.ch () != EOF)
      {
        if (! pDecl_0 ())
	  {
            string msg ("Parse error before \"" + lx.nextChars (10) + "\".");
            error (msg);
            lx.zap ();
	  }
      }
    return 0;
}

/* Maintain error count +
   Make an error message from error text
   This message is stored for future usage
   However, it is currently never used */
void Parser::error (const string& txt)
{
    // only keep the first one
    if (first_error_message == "")
      {
        int line;
        line = lx.getLineNum ();
        ostringstream str;
        str << file << ":" << line << ": error: " << txt << endl;
        first_error_message = str.str ();
      }
    ++errCount;
}

bool Parser::pDecl_0 ()
{
    Ctype ty, root;
    string id;
    bool isextern;
    bool isinterrupt;
    bool isstatic;
    bool istype;
    bool isconst;

    isextern = isstatic = isinterrupt = istype = false;
    blk ();
    isinterrupt = lx.kmatch ("__interrupt__");
    blk ();
    // these qualifiers are mutually exclusive
    (isextern =  lx.kmatch ("extern")) || (isstatic = lx.kmatch ("static"))
                                       || (istype = lx.kmatch ("typedef"));
    blk ();

    isconst = lx.kmatch ("const");
    blk ();

    if (! parseBaseType (root))
      return false;

    // special case of structs or union definition (no identifier)
    blk ();
    if (lx.ch () == ';')
      {
        lx.gch ();
        return true;
      }

    // apply modifiers
    if (isinterrupt)
      root->modifiers () = "__interrupt__ ";
    else if (istype)
      root->modifiers () = "typedef ";
    else if (isstatic)
      root->modifiers () = "static ";
    else if (isextern)
      root->modifiers () = "extern ";

    if (isconst)
      root->modifiers () = "const ";

    // const might be placed before or after type
    for (;;)
      {
        ty = root;

        blk ();
        if (lx.kmatch ("const"))
	  ty->modifiers () += "const ";

        if (pDecl_1 (ty, id))
	  {
            // definition or declaration found
            int symbolline = lx.getLineNum ();
            int symboltype = SymbEntry::Unknown;

            // fct always extern
            if (ty->is (type::Fct))
	      {
                symboltype |= SymbEntry::Function;
                blk ();
                if (lx.ch () == '{')
		  {
                    lx.gch ();
                    zapBlock ('{', '}');
                    symboltype |= SymbEntry::Definition;
                    // push back a fake ';'
                    lx.pushBack (";");
		  }
	      }
            else if (ty->is (type::Array))
	      {
                symboltype |= SymbEntry::Variable;
                if (! isextern)
		  symboltype |= SymbEntry::Definition;
                blk ();
                if (lx.ch () == '=')
		  {
                    lx.gch (); blk ();
                    if (lx.ch () == '{')
		      {
                        lx.gch ();
                        zapBlock ('{', '}');
		      }
                    else
		      return false;
		  }
	      }
            else if (ty->is (type::Ptr))
	      {
                symboltype |= SymbEntry::Variable;
                if (! isextern)
		  symboltype |= SymbEntry::Definition;
                zapInitialization ();
	      }
            else if (! istype)
	      {
                symboltype |= SymbEntry::Variable;
                if (! isextern)
		  symboltype |= SymbEntry::Definition;
                zapInitialization ();
	      }
            else
	      {
                // a type name cannot be initialized
                symboltype |= SymbEntry::Typename;
                // typedef alwaws considered as a declaration
	      }
            // store symbol with source code line number, symbol type,
	    // symbol name and pretty printed declaration
            symbTable.push_back (SymbEntry (symbolline, symboltype,
					    id, ty->prettyPrint ("", id)));

            blk ();
            if (! lx.match (","))
	      break;
	  }
        else
	  return false;
      }
    return ns ();
}

bool Parser::zapKeyword (const char *key, string& zapped)
{
    bool r = false;
    if ((r = lx.kmatch (key)))
      {
        zapped += key;
        zapped += " ";
      }
    return r;
}

// parse classic base type and long/short flavors
bool Parser::parseComposedTypes (string& name)
{
    name = "";
    zapKeyword ("short", name) || zapKeyword ("long", name);

    blk ();

    zapKeyword ("int", name) || zapKeyword ("long", name)
                             || zapKeyword ("float", name)
                             || zapKeyword ("double", name);

    return name != "";
}

/* parse any type name (including struct or union)
   TODO : anonymous union ?? */
bool Parser::parseBaseType (Ctype& t)
{
    string name;
    string structness, signess, longness;
    bool r = false;
    blk ();

    zapKeyword ("struct", structness) || zapKeyword ("union", structness);
    blk ();

    zapKeyword ("signed", signess) || zapKeyword ("unsigned", signess);
    blk ();

    if (parseComposedTypes (name) || lx.ident (name))
      {
        t = Ctype (type::newtype (type::Terminal, signess + structness + name));
        r = true;
      }
    blk ();

    // zap structure definition, if any
    if (structness != "" && lx.ch () == '{')
      {
        lx.gch ();
        zapBlock ('{', '}');
      }

    return r;
}

bool Parser::pDecl_1 (Ctype& typ, string& id)
{
    while (blk (), lx.ch () == '*')
      {
        lx.gch ();
        lx.kmatch ("const"); // const modifier ignored
        Ctype sub = Ctype (type::Ptr, "");
        sub->subtype () = typ;
        typ = sub;
      }

    return pDecl_2 (typ, id);
}

bool Parser::pDecl_2 (Ctype& typ, string& id)
{
    Ctype type2;
    string id2;
    int useraddr = -1;

    string arraySize;

    blk ();
    if (lx.ident (id))
      {
        // special support for "placed" entities
        if (lx.match ("@") && lx.number (useraddr))
	  {
            ostringstream x;
            x << " [@0x" << hex << useraddr << "]";
            id += x.str ();
	  }
      }
    else if (lx.match ("(") && pDecl_1 (type2, id2) && blk () && lx.match (")"))
      ;
    else
      return false;

    Ctype arrays;
    /* Array size can be empty (equivalent to size == 0)
       or must be an arbitrary complex expression */
    while (blk () && lx.match ("[") && zapTo ('[', ']', arraySize))
      {
        Ctype sub (type::Array);
        sub->args () = arraySize;
        arraySize = "";
        if (arrays)
	  arrays->terminaltype () = sub;
        else
	  arrays = sub;
      }

    Ctype function, func;
    string funcParams;
    if (blk () && lx.match ("(") && zapTo ('(', ')', funcParams))
      {
        function = Ctype (type::Fct);
        function->args () = funcParams;
      }
    if (function)
      {
        function->subtype () = typ;
        typ = function;
      }
    if (arrays)
      {
        arrays->terminaltype () = typ;
        typ = arrays;
      }
    if (type2)
      {
        type2->terminaltype () = typ;
        typ = type2;
        id = id2;
      }
    return true;
}

// Warning : can consume all the stream in case of syntax error
bool Parser::zapTo (int open, int close, string& inside)
{
    int k = 1;
    for (; lx.ch () != EOF && k != 0; lx.gch ())
      {
        zapComment ();
        if (lx.ch () != close)
	  {
            inside += (char) lx.ch ();
            if (lx.ch () == open)
	      ++k;
	  }
        else
	  --k;
      }
    return k == 0;
}

// Zap a block, (does not keep characters inside braces)
bool Parser::zapBlock (int open, int close)
{
    int k = 1;
    for (; lx.ch () != EOF && k != 0; lx.gch ())
      {
        while (zapComment () || zapLiteral () || zapCharConstant ())
	  ;
        if (lx.ch () != close)
	  {
            if (lx.ch () == open)
	      ++k;
	  }
        else
	  --k;
      }
    return k == 0;
}

// zap /* */ comments AND // comments
bool Parser::zapComment ()
{
    bool r = false;
    if (lx.match ("/*"))
      {
        r = true;
        while (lx.ch () != EOF)
	  {
            if (lx.match ("*/"))
	      break;
            lx.gch ();
	  }
      }
    else if (lx.match ("//"))
      {
        r = true;
        while (lx.ch () != EOF && lx.ch () != '\n')
	  lx.gch ();
      }
    return r;
}

// zap "literal strings"
bool Parser::zapLiteral ()
{
    bool r = false;
    int lastc = 0;

    if (lx.ch () == '"')
      {
        r = true;
        while (lastc = lx.gch (), (lx.ch () != EOF))
	  {
            if (lx.ch () == '"' && lastc != '\\')
	      break;
	  }
        lx.gch (); // get trailing " (if EOF - does nothing)
      }
    return r;
}

// zap 'X'
bool Parser::zapCharConstant ()
{
    bool r = false;
    int lastc = 0;

    if (lx.ch () == '\'')
      {
        r = true;
        while (lastc = lx.gch (), (lx.ch () != EOF))
	  {
            if (lx.ch() == '\'' && lastc != '\\')
	      break;
	  }
        lx.gch (); // get trailing ' (if EOF - does nothing)
      }
    return r;
}

// semicolon enforcer
bool Parser::ns ()
{
    blk ();
    if (lx.match (";"))
      return true;

    string msg ("Parse error before \"" + lx.nextChars (5) + "\" (missing semicolon ?)");
    error (msg);
    return false;
}

// eat blanks and comments
bool Parser::blk ()
{
    while (lx.needblk () || zapComment ())
      ;
    return true;
}

/* An initialization begins with =
   and stops with ',' or ';'
   (',' inside () or {} or "" or '' are not considered as end markers) */
bool Parser::zapInitialization ()
{
    bool r = false;
    int pc, bc;

    blk ();
    if (lx.ch () == '=')
      {
        r = true;
        blk ();
        for (bc = pc = 0; lx.ch () != EOF; lx.gch ())
	  {
            int c = lx.ch ();
            if (c == ';')
	      break;
            else if (c == ',' && bc == 0 && pc == 0)
	      break;
            else if (zapCharConstant () || zapLiteral () || zapComment ())
	      continue;
            else if (c == '{')
	      ++bc;
            else if (c == '(')
	      ++pc;
            else if (c == '}')
	      --bc;
            else if (c == ')')
	      --pc;
	    else
	      { ; } // nothing to do
	  }
      }
    return r;
}
