/* Helper class for code analyzer */

#ifndef CODEANALYZER_H
#define CODEANALYZER_H

#include <vector>
using namespace std;

#include <QString>
#include <QObject>

#include "codeeditor/QBEditor.h"


struct EntryPointRecord
{
    QString name, prototype;
    unsigned int line;

    EntryPointRecord (const QString& name_, const QString& prototype_,
		      unsigned int line_)
        : name (name_), prototype (prototype_), line (line_) 
    {
    }
};

/* Base class for code analyzers
   A code analyzer detects entry points and return
   a) name
   b) prototype
   c) line number
@author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */
class CodeAnalyzer : public QObject
{
    Q_OBJECT

protected:
    vector<EntryPointRecord> entryPoints_;

public:
    CodeAnalyzer (QObject *parent = 0);
    ~CodeAnalyzer ();
    virtual int analyze (QBEditorDocument *doc);
    vector<EntryPointRecord>& entryPoints ();
};

#endif
