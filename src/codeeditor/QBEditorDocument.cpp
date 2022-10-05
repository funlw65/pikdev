#include <iostream>
using namespace std;

#include <QFile>
#include <QPlainTextDocumentLayout>
#include <QTextStream>
#include <QTimer>

#include "../FNU.h"
#include "QBEditor.h"
#include "SHlFactory.h"


QBEditorDocument::QBEditorDocument (QObject *parent, const QUrl& u) :
    QTextDocument (parent), url_ (u),
    currentLine (1), prevLine (1), prevColumn (1),
    currentEditor (0), readonly_ (false)
{  
    QPlainTextDocumentLayout *lay = new QPlainTextDocumentLayout (this);
    setDocumentLayout (lay);
    connect (this, SIGNAL (contentsChanged ()), this, SLOT (slotNotAnalyzed ()));

    setEditorFontSize (QBEditor::editorFontSize ());
    setEditorFont (QBEditor::editorFont ());
}

void QBEditorDocument::setEditorFontSize (int sz)
{
    QFont font = defaultFont ();
    font.setPointSize (sz);
    setDefaultFont (font);
}

void QBEditorDocument::setEditorFont (const QString& fname)
{
    QFont font = defaultFont ();
    font.setFamily (fname);
    setDefaultFont (font);
}

void QBEditorDocument::slotNotAnalyzed ()
{
    setAnalyzed (false);
}

void QBEditorDocument::setReadOnly (bool r)
{
    readonly_ = r;
}

bool QBEditorDocument::isReadOnly ()
{
    return readonly_;
}

void QBEditorDocument::setURL (const QUrl& u)
{
    url_ = u;
}

CodeEditor *QBEditorDocument::editor ()
{
    return currentEditor;
}

void QBEditorDocument::setEditor (CodeEditor *e)
{
    // disconnect the previous document, if it exists
    if (currentEditor != 0)
      {
        QBEditorDocument *prevdoc = (QBEditorDocument *) e->document ();
        if (prevdoc)
	  {
            if (prevdoc == this)
	      return; // editor is already connected to this doc
            prevdoc->currentEditor = 0;
	  }
      }
    currentEditor = e;
    e->setDocument (this);
    e->setSyntaxHL (SHlFactory::factory ()->getSHl (fileFullName ()));
    e->setReadOnly (isReadOnly ());
}

QString QBEditorDocument::fileName ()
{
    QString pathfile = FNU::noScheme (url_);
    int k = pathfile.lastIndexOf (QChar ('/'));
    if (k == -1)
      pathfile.lastIndexOf (QChar ('\\')); // DOS flavor
    if (k == -1)
      return pathfile;
    return pathfile.right (pathfile.size () - k - 1);
}

QString QBEditorDocument::path ()
{
    QString pathfile = FNU::noScheme (url_);
    int k = pathfile.lastIndexOf (QChar ('/'));
    if (k == -1)
      pathfile.lastIndexOf (QChar ('\\')); // DOS flavor
    if (k == -1)
      return "";
    return pathfile.left (k + 1);
}

QString QBEditorDocument::fileFullName ()
{
    return FNU::noScheme (url_);
}

QString QBEditorDocument::loadFile (const QUrl& u)
{
    QString fn = FNU::noScheme (u);
    QFile file (fn);
    if (! file.open (QIODevice::ReadOnly))
      return tr ("File %1 cannot be opened").arg (fn);
    QTextStream stream (&file);

    setPlainText (stream.readAll ());
    setModified (false);
    url_ = u;
    // note: syntax HL engine setting (and other updates of editor) is done by setEditor ()

    return "";
}

QString QBEditorDocument::saveFileAs (const QUrl& u)
{
    QString fn = FNU::noScheme (u);
    QFile file (fn);
    if (! file.open (QIODevice::WriteOnly))
      return tr ("File %1 cannot be opened").arg (fn);

    QTextStream stream (&file);

    if (currentEditor)
      currentEditor->setSyntaxHL (SHlFactory::factory ()->getSHl (fn));
    stream << toPlainText ();
    setModified (false);
    url_ = u;

    return "";
}

QString QBEditorDocument::saveFile ()
{
    QString fn = FNU::noScheme (url_);
    QFile file (fn);
    if (! file.open (QIODevice::WriteOnly))
      return tr ("File %1 cannot be opened").arg (fn);

    QTextStream stream (&file);

    if (currentEditor)
      currentEditor->setSyntaxHL (SHlFactory::factory ()->getSHl (fn));
    stream << toPlainText ();
    setModified (false);
    return "";
}

const QUrl& QBEditorDocument::url ()
{
    return url_;
}

bool QBEditorDocument::analyzed ()
{
    return analyzed_;
}

void QBEditorDocument::setAnalyzed (bool a)
{
    analyzed_ = a;
}

QString QBEditorDocument::getLineText (int i)
{
    return findBlockByNumber (i).text ();
}
