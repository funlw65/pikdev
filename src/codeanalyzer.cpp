#include <iostream>
using namespace std;

#include "codeanalyzer.h"
#include "codeeditor/QBEditor.h"


CodeAnalyzer::CodeAnalyzer (QObject *parent) : QObject (parent)
{
}

CodeAnalyzer::~CodeAnalyzer ()
{
}

int CodeAnalyzer::analyze (QBEditorDocument *doc)
{
    entryPoints_.clear ();

    for (int i = 0; i < doc->blockCount (); ++i)
      {
        QString line = doc->getLineText (i);
        if (line.startsWith (";<+"))
	  {
            QString symbol, proto;
            int k;
            for (k = 3; k < line.length () && line[k] != '>' && line[k] != '|'; ++k)
	      symbol += line[k];
            // is prototype here ?
            if (++k < line.length ())
	      proto = line.right (line.length () - k);
            entryPoints_.push_back (EntryPointRecord (symbol, proto, i));
	  }
      }
    return 0;
}

vector<EntryPointRecord>& CodeAnalyzer::entryPoints ()
{
    return entryPoints_;
}
