/***************************************************************************
    begin                : 2010
    copyright            : 2010 by Alain Gibaud
    email                : alain.gibaud@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Ctype.h"
#include "Lexer.h"
#include "codeeditor/QBEditor.h"


// An entry of the symbol table
struct SymbEntry
{
    enum SymbolType {Unknown = 0, Definition = 1, Function = 2, Variable = 4, Typename = 8};

    int line; // line number for this symbol
    int symbol_type; // true == declaration, else definition
    string name; // symbol name
    string prototype; // full prototype, rewritten from type tree by prettyPrint ()	

    SymbEntry (int _line, int _symbol_type, const string& _name, const string& _prototype)
        : line (_line), symbol_type (_symbol_type), name (_name), prototype (_prototype)
    {}
};

// Parser for C entities declaration / definition
class Parser
{
public:
    Parser ();
    ~Parser ();
    int parse (QBEditorDocument *doc);
    void error (const string& txt);
    bool pDecl_0 ();
    bool pDecl_1 (Ctype& typ, string& id);
    bool pDecl_2 (Ctype& typ, string& id);
    // zap everything between open and close 
    // return parsed string
    bool zapTo (int open, int close, string& inside);
    bool zapBlock (int open, int close);
    bool parseBaseType (Ctype& t);
    bool ns ();
    bool blk ();
    bool zapComment ();
    bool zapLiteral ();
    bool zapCharConstant ();
    bool zapInitialization ();
    vector<SymbEntry>& symbols () { return symbTable; } 
    int getErrCount () { return errCount; }
    const string& getErrMessage () { return first_error_message; }
    bool zapKeyword (const char *key, string& zapped);
    bool parseComposedTypes (string& name);	

private:
    Lexer lx;	
    ifstream *stream;
    string file; 
    int errCount;
    vector<SymbEntry> symbTable;
    string first_error_message;
};

#endif
