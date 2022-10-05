#include <iostream>
using namespace std;

#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QHash>
#include <QMessageBox>
#include <QtGui>

#include "../FNU.h"
#include "../RecentItemsAction.h"
#include "ItemSelector.h"
#include "QBEditor.h"
#include "SHlFactory.h"
#include "Utility.h"


// TODO : find another mechanism to implement this
int QBEditor::fontSize_ = 12;
QString QBEditor::fontName_ = "DejaVu Sans";

QBEditor::QBEditor (QWidget *parent) : QFrame (parent)
{
    displayPaths = false; // short names by default
    QColor mygrey = QColor (0xA0, 0xA0, 0xA0);

    QGridLayout *lay = new QGridLayout (this);
    lay->setMargin (1);
    lay->setSpacing (1);

    QFrame *navbar = new QFrame (this);
    QPalette p = navbar->palette ();
    p.setColor (QPalette::Window, mygrey);
    navbar->setPalette (p);
    navbar->setAutoFillBackground (true);

    lay->addWidget (navbar, 0, 0, 1, 3);
    QGridLayout *navlay = new QGridLayout (navbar);
    navlay->setMargin (0);
    navlay->setSpacing (1);

    QPushButton *showpath = new QPushButton (QIcon (":/src/icons/22/show-more.png"),
					     QString (), navbar);
    showpath->setFixedSize (QSize (22, 22));
    showpath->setFlat (true);
    navlay->addWidget (showpath, 0, 0);

    documentSelector = new QComboBox (navbar);
    QPalette ps = documentSelector->palette ();
    ps.setColor (QPalette::Window, mygrey);
    documentSelector->setPalette (ps);
    documentSelector->setAutoFillBackground (true);

    navlay->addWidget (documentSelector, 0, 1);

    QFrame *sep = new QFrame (navbar);
    sep->setFixedSize (QSize (10, 2));
    navlay->addWidget (sep, 0, 2);

    lineSelector = new QSpinBox (navbar);
    lineSelector->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    navlay->addWidget (lineSelector, 0, 3);
    lineSelector->setVisible (false);

    statusLabel = new QLabel (navbar);
    statusLabel->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    navlay->addWidget (statusLabel, 0, 4);

    QPushButton *close = new QPushButton (QIcon (":/src/codeeditor/icons/dialog-close.png"),
					  "", navbar);
    close->setFixedSize (QSize (22, 22));
    close->setFlat (true);
    navlay->addWidget (close, 0, 5);
    close->setToolTip (tr ("Close current document"));

    editor = new CodeEditor (this);
    lay->addWidget (editor, 1, 0, 1, 3);

    searchDialog = new QBEditorSearchDialog (this);
    lay->addWidget (searchDialog, 2, 0, 1, 3);
    connect (searchDialog, SIGNAL (sigSearch (const QString&, bool, bool)),
	     editor, SLOT (slotSearch (const QString&, bool, bool)));
    connect (searchDialog, SIGNAL (sigReplace (const QString&)),
	     editor, SLOT (slotReplace (const QString&)));
    connect (searchDialog, SIGNAL (sigSearchReplace (const QString&, const QString&, bool, bool)),
	     editor, SLOT (slotSearchReplace (const QString&, const QString&, bool, bool)));
    connect (searchDialog, SIGNAL (sigHide ()), editor, SLOT (setFocus ()));
    searchDialog->slotSetVisibility (false);

    connect (editor, SIGNAL (statusChanged ()), this, SLOT (slotUpdateStatusLine ()));
    connect (lineSelector, SIGNAL (valueChanged (int)), editor, SLOT (slotGotoLine (int)));
    connect (lineSelector, SIGNAL (editingFinished ()), this, SLOT (slotStopGotoLine ()));

    connect (documentSelector, SIGNAL (currentIndexChanged (int)), this,
	     SLOT (slotSelectDocument (int)));
    connect (close, SIGNAL (clicked ()), this, SLOT (slotCloseCurrentDocument ()));

    connect (this, SIGNAL (sigMustDeleteDocument (QBEditorDocument *)), this,
	     SLOT (slotDeleteDocument (QBEditorDocument *)));
    connect (showpath, SIGNAL (clicked ()), this, SLOT (slotToggleShowPaths ()));

    setupActions ();
    slotUpdateStatusLine ();
    editor->setTab (3);
    QSettings settings;
    restoreSettings (settings);
    lastDocIndex = -1;
}

QBEditor::~QBEditor ()
{
    for (int i = 0; i < documentsCount (); ++i)
      {
        QBEditorDocument *doc
	  = (QBEditorDocument *) Utility::strToP (documentSelector->itemData (i).toString ());
        if (doc)
	  delete doc;
      }
}

void QBEditor::slotUpdateDocumentSelector (int i)
{
    static QIcon saveIcon (":/src/codeeditor/icons/document-save.png");
    static QIcon nullIcon;
    QBEditorDocument *doc = document (i);
    if (doc)
      documentSelector->setItemIcon (i, doc->isModified () ? saveIcon : nullIcon);
}

void QBEditor::slotUpdateStatusLine ()
{
    int l, c;
    editor->getCursorPosition (l, c);
    QString mod = editor->getLanguage ();
    statusLabel->setText (QString ("<b>Line</b>: %1, <b>Col</b>: %2, <b>Mode</b>: %3")
			  .arg (l).arg (c).arg (mod));

    // TODO : separate this signal
    slotUpdateDocumentSelector (curDocNumber ());
}

void QBEditor::slotNew ()
{
    QFileDialog dialog (this, tr ("Enter new file name"));
    dialog.setFileMode (QFileDialog::AnyFile);
    dialog.setAcceptMode (QFileDialog::AcceptOpen);
    dialog.setNameFilter ("*");
    dialog.setViewMode (QFileDialog::Detail);
    dialog.setDirectory (defaultDir);

    if (dialog.exec ())
      defaultDir = dialog.directory ();
    QStringList list = dialog.selectedFiles ();
    QString f;
    if (list.count () == 1)
      f = list.first ();

    // file exists ?
    QFile file (f);
    if (file.exists ())
      {
        QMessageBox::critical (this, tr ("Sorry"), tr ("This file already exists."));
        return;
      }

    if (f != "")
      {
        // the QBEditor is the parent, so the document is NOT managed by CodeEditor
        QUrl u (f);
        QBEditorDocument *doc = document (u);
        doc = new QBEditorDocument (this, u);
        doc->setEditor (editor);
        doc->loadFile (u);
        doc->setModified (true);
        editor->setReadOnly (false);
        documentSelector->addItem (doc->fileFullName (),
				   QVariant (Utility::pToStr ((void*) doc)));
        documentSelector->setCurrentIndex (documentSelector->count () - 1);
        slotUpdateStatusLine ();
      }
}

/* ----------------------------------------------------------------------- 
  Load a file (or reload it, when it is already loaded)

  Returns:
  the attached document, or NULL if the document cannot be loaded
  ----------------------------------------------------------------------- */
QBEditorDocument *QBEditor::loadReloadFile (const QUrl& url)
{
    QBEditorDocument *doc;

    if ((doc = document (url)) ==  0) // does not exist
      {
        doc = new QBEditorDocument (this, url);
        if (doc->loadFile (url) == "")
	  {
            doc->setEditor (editor);
            documentSelector->addItem (displayName (doc),
				       QVariant (Utility::pToStr ((void*) doc)));
            documentSelector->setCurrentIndex (documentSelector->count () - 1);
            slotUpdateStatusLine ();
            // update the "recent files" menu
            RecentItemsAction *ra = dynamic_cast<RecentItemsAction *>(act ("open-recent"));
            if (ra)
	      ra->addEntry (FNU::noScheme (url));
	  }
        else
	  {
            delete doc;
            doc = 0;
	  }
      }
    else // reload
      {
        if (doc->loadFile (url) == "")
	  {
            setDocument (doc);
            documentSelector->setCurrentIndex (documentIndex (url));
            slotUpdateStatusLine ();
	  }
      }
    return doc;
}

// Another way to execute loadReloadFile (const QUrl& url)
void QBEditor::slotLoadReloadFile (const QString& filename)
{
    loadReloadFile (QUrl (filename));
}

// switch to document specidied by url, or create this document
// if it doesn't exist
QBEditorDocument *QBEditor::switchOrLoad (const QUrl& url)
{
    QBEditorDocument *doc;

    if ((doc = document (url)) == 0) // does not exist
      {
        doc = new QBEditorDocument (this, url);
        doc->loadFile (url);
        documentSelector->addItem (displayName (doc),
				   QVariant (Utility::pToStr ((void*) doc)));
        int item = documentSelector->count () - 1;
        documentSelector->setCurrentIndex (item);
        editor->slotGotoPos (1, 1);
        // update the "recent files" menu
        RecentItemsAction *ra = dynamic_cast<RecentItemsAction *>(act ("open-recent"));
        if (ra)
	  ra->addEntry (FNU::noScheme (url));
      }
    else // exists : switch editor to this document
      documentSelector->setCurrentIndex (documentIndex (doc));
    return doc;
}

void QBEditor::slotZoomIn ()
{
    setEditorFontSize (editorFontSize () + 1);
}

void QBEditor::slotZoomOut ()
{
    setEditorFontSize (editorFontSize () - 1);
}

void QBEditor::slotLoadFile ()
{
    QFileDialog dialog (this, tr ("Select files to load"));
    dialog.setFileMode (QFileDialog::ExistingFiles);
    dialog.setAcceptMode (QFileDialog::AcceptOpen);
    dialog.setNameFilter ("*");
    dialog.setViewMode (QFileDialog::Detail);
    dialog.setDirectory (defaultDir);

    if (dialog.exec ())
      defaultDir = dialog.directory ();
    QStringList list = dialog.selectedFiles ();

    QStringList::const_iterator it;

    for (it = list.begin (); it != list.end (); ++it)
      {
        QString f = *it;

        if (f != "")
	  {
	    // the QBEditor is the parent, so the document is NOT managed by CodeEditor
            QUrl u (f);
            QBEditorDocument *doc = document (u);
            if (doc == 0)
	      loadReloadFile (u);
            else
	      {
                int rep = QMessageBox::question (this, tr ("Information"),
						 tr ("This file is already loaded. Reload ?<br>"
						     "(The current buffer will be lost)"),
						 QMessageBox::Yes | QMessageBox::No,
						 QMessageBox::Yes);
                if (rep == QMessageBox::Yes)
		  loadReloadFile (u);
	      }
	  }
      }
}

void QBEditor::slotSaveFileAs ()
{
    QBEditorDocument *doc = currentDocument ();
    if (doc)
      {
        QFileDialog dialog (this, tr ("File to save"));
        dialog.setFileMode (QFileDialog::AnyFile);
        dialog.setAcceptMode (QFileDialog::AcceptSave);
        dialog.setNameFilter ("*");
        dialog.setViewMode (QFileDialog::Detail);
        dialog.setDirectory (defaultDir);

        if (dialog.exec ())
	  defaultDir = dialog.directory ();
        QStringList li = dialog.selectedFiles ();

        if (li.count () != 0)
	  {
            QBEditorDocument *doc = currentDocument ();

            doc->saveFileAs (QUrl (li.first ()));
            documentSelector->setItemText (curDocNumber (), doc->fileName ());
            slotUpdateStatusLine ();
	  }
      }
}

void QBEditor::slotSaveFile ()
{
    QBEditorDocument *doc = currentDocument ();
    if (doc)
      {
        QString fn = doc->fileFullName ();

        if (fn == "")
	  slotSaveFileAs ();
        else
	  doc->saveFile ();
        slotUpdateStatusLine ();
      }
}

void QBEditor::slotSaveAllFiles ()
{
    for (int i = 0; i < documentsCount (); ++i)
      {
        QBEditorDocument *doc = document (i);
        if (doc)
	  {
            QString fn = doc->fileFullName ();

            if (fn == "")
	      slotSaveFileAs ();
            else
	      doc->saveFile ();
            slotUpdateStatusLine ();
            slotUpdateDocumentSelector (i);
	  }
      }
}

void QBEditor::slotToggleSearch ()
{
    bool v = searchDialog->isVisible ();
    searchDialog->slotSetVisibility (! v);
}

void QBEditor::slotShowBookmarksMenu ()
{
    QMenu *menu = dynamic_cast<QMenu *>(sender ());
    if (menu == 0)
      return;

    QVector<int> lines = editor->markedLines ();
    menu->clear ();

    QVector<int>::iterator b;

    for (b = lines.begin (); b != lines.end (); ++b)
      {
        QString linetxt = editor->getLineText (*b);
        linetxt.chop (1); // remove trailing newline
        linetxt.truncate (48);
        linetxt.append ("...");
        QAction *a = menu->addAction (QString ("%1 - \"%2\"").arg ((*b) + 1).arg (linetxt));
        connect (a, SIGNAL (triggered ()), this, SLOT (slotGotoBookmark ()));
      }
}

void QBEditor::slotGotoBookmark ()
{
    QAction *act = dynamic_cast<QAction *>(sender ());
    if (act == 0)
      return;

    QString txt (act->text ());
    // fetch line number in text
    int n, k;
    for (n = k = 0; txt[k].isDigit (); ++k)
      n = n * 10 + (txt[k].toLatin1 () - '0');
    editor->slotGotoLine (n);
}

void QBEditor::slotFlashCurrentLine ()
{
    editor->slotFlashCurrentLine ();
}

void QBEditor::slotFlashLine (int i)
{
    editor->slotGotoLine (i);
    editor->slotFlashCurrentLine ();
}

void QBEditor::slotUndo ()
{
    editor->undo ();
}

void QBEditor::slotRedo ()
{
    editor->redo ();
}

void QBEditor::slotCut ()
{
    editor->cut ();
}

void QBEditor::slotCopy ()
{
    editor->copy ();
}

void QBEditor::slotPaste ()
{
    editor->paste ();
}

void QBEditor::slotGotoLine ()
{
    lineSelector->setRange (1, editor->document ()->blockCount ());
    lineSelector->setVisible (! lineSelector->isVisible ());
}

void QBEditor::slotStopGotoLine ()
{
    lineSelector->setVisible (false);
}

void QBEditor::slotCloseCurrentDocument ()
{
    int k = documentSelector->currentIndex ();
    QBEditorDocument *doc;
    if ((doc = document (k)))
      {
        documentSelector->removeItem (k);
        emit sigMustDeleteDocument (doc);
      }
    editor->setReadOnly (documentsCount () == 0);
}

void QBEditor::slotCloseDocument (int k)
{
    QBEditorDocument *doc;
    if ((doc = document (k)))
      {
        documentSelector->removeItem (k);
        emit sigMustDeleteDocument (doc);
      }
    editor->setReadOnly (documentsCount () == 0);
}

void QBEditor::slotCloseDocument (const QUrl& url)
{
    slotCloseDocument (documentIndex (url));
}

void QBEditor::slotCloseAllDocuments ()
{
    while (documentsCount ())
      slotCloseCurrentDocument ();
    slotUpdateStatusLine ();
}

void QBEditor::slotDeleteDocument (QBEditorDocument *doc)
{
    delete doc;
}

void QBEditor::slotSelectDocument (int k)
{
    if (k == -1)
      {
        // no more documents :
        // create an empty document, with CodeEditor as parent
        QUrl u ("");
        QBEditorDocument *dummy = new QBEditorDocument (editor, u);
        editor->setDocument (dummy);
        slotUpdateStatusLine ();
        return;
      }

    QBEditorDocument *doc;
    // now, change to new document
    if ((doc = document (k)))
      setDocument (doc); // change editor's document
}

void QBEditor::slotToggleLineNumbers ()
{
    editor->setLineNumbersEnabled (! editor->lineNumbersEnabled ());
    QAction *a = (QAction *) sender ();
    a->setChecked (editor->lineNumbersEnabled ());
}

void QBEditor::slotToggleBookmarks ()
{
    editor->setBookmarksEnabled (! editor->bookmarksEnabled ());
    QAction *a = (QAction *) sender ();
    a->setChecked (editor->bookmarksEnabled ());
}

// change to a document that is already in the document selector
void QBEditor::setDocument (QBEditorDocument *doc)
{
    if (lastDocIndex != -1)
      {
        QBEditorDocument *prevDoc = document (lastDocIndex);

        if (prevDoc)
	  editor->getCursorPosition (prevDoc->prevLine, prevDoc->prevColumn);
      }

    doc->setEditor (editor);
    // try to update document selector
    int k = lastDocIndex = documentIndex (doc);
    if (k != -1)
      {
        editor->slotGotoPos (doc->prevLine, doc->prevColumn);
        slotUpdateStatusLine ();
      }
}

void QBEditor::slotCommentBlock ()
{
    editor->comment ();
}

void QBEditor::slotUnCommentBlock ()
{
    editor->unComment ();
}

QBEditorDocument *QBEditor::document (int k)
{
    QBEditorDocument *doc = 0;
    if (k != -1)
      doc = (QBEditorDocument *) Utility::strToP (documentSelector->itemData (k).toString ());
    return doc;
}

int QBEditor::documentIndex (QBEditorDocument *doc)
{
    for (int i = 0; i < documentsCount (); ++i)
      {
        if (doc == document (i))
	  return i;
      }
    return -1;
}

int QBEditor::documentIndex (const QUrl& u)
{
    QBEditorDocument *doc = 0;

    for (int i = 0; i < documentsCount (); ++i)
      {
        doc = document (i);
        if (doc && (doc->url () == u))
	  return i;
      }
    return -1;
}

QBEditorDocument *QBEditor::document (const QUrl& u)
{
    QBEditorDocument *doc = 0;

    for (int i = 0; i < documentsCount (); ++i)
      {
        doc = document (i);
        if (doc && (doc->url () == u))
	  return doc;
      }
    return 0;
}

QBEditorDocument *QBEditor::currentDocument ()
{
    QBEditorDocument *doc = 0;
    int num = curDocNumber ();
    if (num != -1)
      doc = (QBEditorDocument *) Utility::strToP (documentSelector->itemData (num).toString ());
    return doc;
}

int QBEditor::documentsCount ()
{
    return documentSelector->count ();
}

/* ----------------------------------------------------------------------- 
   Generic code for source indentation
   Each type of code can be indented by an external tool
   whose invocation command is provided by the syntax highligther.
   The name of the file to be intented is injected by the standard Qt arg()
   mechanism.
   a) No tool is available to indent ASM code, but this tool is very simple
      to write.
   b) C/C++ source code is currently indentable with "astyle".
   ----------------------------------------------------------------------- */
void QBEditor::slotIndent ()
{
    QBEditorDocument *doc = currentDocument ();

    if (! doc)
      return;

    QString fn = doc->fileFullName ();
    HighlighterBase *hl = SHlFactory::factory ()->getSHl (fn);

    if (hl && ! doc->isReadOnly ())
      {
        QString indentCmd = hl->indenter ().arg (fn);

        if (indentCmd != "")
	  {
            int l, c;
            if (doc->isModified ())
	      doc->saveFile ();
            editor->getCursorPosition (l, c);

            FILE *f = popen (indentCmd.toLatin1 (), "r");
            if (f)
	      {
                pclose (f);
                doc->loadFile (QUrl (fn));
                editor->setCursorPosition (l, c);
	      }
	  }
        else
	  {
            QMessageBox::warning (this, tr ("Cannot indent"),
				  tr ("No indenter for this kind of document"));
	  }
      }
}

void QBEditor::slotAboutToClose ()
{
    int unsaved = 0;

    // count unsaved buffers
    for (int k = 0; k < documentsCount (); ++k)
      {
        QBEditorDocument *e = document (k);
        if (e->isModified ())
	  ++unsaved;
      }

    if (unsaved)
      {
        ItemSelector sel (tr ("The following files are not saved."), this,
			  tr ("Save all"), tr ("Save selected"), tr ("Leave all"));
        for (int k = 0; k < documentsCount (); ++k)
	  {
            QBEditorDocument *e = document (k);
            if (e->isModified ())
	      sel.addItem (e->fileName () + "   " + e->path (), k);
	  }
        sel.exec ();

        if (sel.result () != ItemSelector::Cancel)
	  {
            QVector<int> ids = sel.results ();
            for (int k = 0; k < ids.count (); ++k)
	      document (ids[k])->saveFile ();
	  }
      }
}

QBEditorDocument *QBEditor::switchToDocument (const QUrl& u)
{
    QBEditorDocument *d = document (u);
    if (d)
      setDocument (d);
    return d;
}

QStringList QBEditor::getAllFileNames ()
{
    QStringList opened_files;
    for (int i = 0; i < documentsCount (); ++i)
      opened_files += document (i)->fileFullName ();
    return opened_files;
}

QAction *QBEditor::act (const QString& s)
{
    QMap<QString, QAction *>::iterator i = actions.find (s);
    return (i == actions.end ()) ? 0 : *i;
}

QAction *QBEditor::addAction (const QString& name, const QIcon& icon,
			      const QString& text, const QKeySequence& shortcut,
			      const char *slot, QObject *parent)
{
    QMap<QString, QAction *>::iterator i = actions.find (name);
    QAction *a = 0;
    if (i == actions.end ())
      {
        a = new QAction (icon, text, parent);
        a->setShortcut (shortcut);
        actions[name] = a;
        connect (a, SIGNAL (triggered ()), this, slot);
      }
    return a;
}

/* ----------------------------------------------------------------------- 
   Insert an already existing action

   Returns:
   a if success, else 0
   ----------------------------------------------------------------------- */
QAction *QBEditor::addAction (const QString& name, QAction *a)
{
    QMap<QString, QAction *>::iterator i = actions.find (name);
    if (i == actions.end ())
      actions[name] = a;
    else
      a = 0;
    return a;
}

void QBEditor::setupActions ()
{
    QAction *a;

    addAction ("open", QIcon (":/src/codeeditor/icons/open.png"), tr ("&Open"),
	       QKeySequence (tr ("Ctrl+O")), SLOT (slotLoadFile ()), this);

    addAction ("save", QIcon (":/src/codeeditor/icons/save.png"), tr ("&Save"),
	       QKeySequence (tr ("Ctrl+S")), SLOT (slotSaveFile ()), this);

    addAction ("save-as", QIcon (":/src/codeeditor/icons/save-as.png"), tr ("Save &As"),
	       QKeySequence (tr ("Ctrl+A")), SLOT (slotSaveFileAs ()), this);

    addAction ("new", QIcon (":/src/codeeditor/icons/document-new"), tr ("&New file"),
	       QKeySequence (tr ("Ctrl+N")), SLOT (slotNew ()), this);

    a = addAction ("undo", QIcon (":/src/codeeditor/icons/edit-undo.png"), tr ("&Undo"),
		   QKeySequence (tr ("Ctrl+U")), SLOT (slotUndo ()), this);
    a->setEnabled (false);    
    connect (editor, SIGNAL (undoAvailable (bool)), a, SLOT (setEnabled (bool)));

    a = addAction ("redo", QIcon (":/src/codeeditor/icons/edit-redo.png"), tr ("&Redo"),
		   QKeySequence (tr ("Ctrl+R")), SLOT (slotRedo ()), this);
    a->setEnabled (false);    
    connect (editor, SIGNAL (redoAvailable (bool)), a, SLOT (setEnabled (bool)));

    addAction ("copy", QIcon (":/src/codeeditor/icons/edit-copy.png"), tr ("&Copy"),
	       QKeySequence (tr ("Ctrl+C")), SLOT (slotCopy ()), this);

    addAction ("cut", QIcon (":/src/codeeditor/icons/edit-cut.png"), tr ("C&ut"),
	       QKeySequence (tr ("Ctrl+X")), SLOT (slotCut ()), this);

    addAction ("paste", QIcon (":/src/codeeditor/icons/edit-paste.png"), tr ("Pas&te"),
	       QKeySequence (tr ("Ctrl+V")), SLOT (slotPaste ()), this);

    addAction ("goto", QIcon (":/src/codeeditor/icons/go-jump.png"), tr ("&Goto"),
	       QKeySequence (tr ("Ctrl+G")), SLOT (slotGotoLine ()), this);

    addAction ("close", QIcon (":/src/codeeditor/icons/document-close.png"), tr ("Close Document"),
	       QKeySequence (tr ("Ctrl+W")), SLOT (slotCloseCurrentDocument ()), this);

    a = addAction ("bookmarks", QIcon (), tr ("Display &Bookmarks margin"),
		   QKeySequence (Qt::Key_F12), SLOT (slotToggleBookmarks ()), this);
    a->setCheckable (true);
    a->setChecked (editor->bookmarksEnabled ());

    a = addAction ("line-numbers", QIcon (), tr ("Display &line numbers"),
		   QKeySequence (Qt::Key_F11), SLOT (slotToggleLineNumbers ()), this);
    a->setCheckable (true);
    a->setChecked (editor->lineNumbersEnabled ());

    addAction ("comment", QIcon (), tr ("Comment bloc&k"),
	       QKeySequence (tr ("Ctrl+K")), SLOT (slotCommentBlock ()), this);

    addAction ("uncomment", QIcon (), tr ("&Uncomment block"),
	       QKeySequence (tr ("Ctrl+Shift+K")), SLOT (slotUnCommentBlock ()), this);

    addAction ("indent", QIcon (), tr ("&Indent"),
	       QKeySequence (tr ("Ctrl+I")), SLOT (slotIndent ()), this);

    addAction ("search-replace", QIcon (":/src/codeeditor/icons/edit-find-replace.png"),
	       tr ("&Find/replace"), QKeySequence (tr ("Ctrl+F")),
	       SLOT (slotToggleSearch ()), this);

    RecentItemsAction *ra = new RecentItemsAction (tr ("Open Recent"), this);
    addAction ("open-recent", ra);
    connect (ra, SIGNAL (itemSelected (QString)), this,
	     SLOT (slotLoadReloadFile (const QString&)));

    addAction ("close-all", QIcon (), tr ("Close All"),
	       QKeySequence (), SLOT (slotCloseAllDocuments ()), this);

    addAction ("save-all", QIcon (":/src/codeeditor/icons/save-all.png"), tr ("Save All"),
	       QKeySequence (), SLOT (slotSaveAllFiles ()), this);

    addAction ("zoom-in", QIcon (":/src/codeeditor/icons/zoom-in.png"), tr ("Zoom In"),
	       QKeySequence (tr ("Ctrl++")), SLOT (slotZoomIn ()), this);

    addAction ("zoom-out", QIcon (":/src/codeeditor/icons/zoom-out.png"), tr ("Zoom Out"),
	       QKeySequence (tr ("Ctrl+-")), SLOT (slotZoomOut ()), this);
}

// Return the name of the document, according to the displayPaths flag
QString QBEditor::displayName (QBEditorDocument *doc)
{
    if (doc == 0)
      return "";
    return displayPaths ? doc->fileFullName () : doc->fileName ();
}

void QBEditor::slotToggleShowPaths ()
{
    displayPaths = ! displayPaths;
    // update the document selector
    int nf = documentSelector->count ();
    for (int i = 0; i < nf; ++i)
      {
        QBEditorDocument *d
	  = (QBEditorDocument *) Utility::strToP (documentSelector->itemData (i).toString ());
        if (d)
	  documentSelector->setItemText (i, displayName (d));
      }
}

void QBEditor::saveSettings (QSettings& settings)
{
    settings.beginGroup ("Editor");
    settings.setValue ("displayPaths", displayPaths);
    settings.setValue ("defaultDialogDir", defaultDir.absolutePath ());
    settings.setValue ("autoAlign", autoAlign ());
    settings.setValue ("fontSize", editorFontSize ());
    settings.setValue ("fontName", editorFont ());

    settings.endGroup ();
    settings.sync ();
}

void QBEditor::restoreSettings (QSettings& settings)
{
    settings.beginGroup ("Editor");
    displayPaths = settings.value ("displayPaths", false).toBool ();
    defaultDir = QDir (settings.value ("defaultDialogDir", "").toString ());
    setAutoAlign (settings.value ("autoAlign", true).toBool ());
    setEditorFontSize (settings.value ("fontSize", 9).toInt ());
    setEditorFont (settings.value ("fontName", "").toString ());

    settings.endGroup ();
}

void QBEditor::setAutoAlign (bool align)
{
    editor->setAutoAlignText (align);
}

bool QBEditor::autoAlign ()
{
    return editor->autoAlignText ();
}

void QBEditor::setEditorFontSize (int sz)
{
    fontSize_ = sz;
    // change each document
    for (int i = 0; i < documentsCount (); ++i)
      document (i)->setEditorFontSize (sz);
}

int QBEditor::editorFontSize ()
{
    return fontSize_;
}

QString QBEditor::editorFont ()
{
   return fontName_;
}

void QBEditor::setEditorFont (const QString& fontName)
{
    fontName_ = fontName;
    // change each document
    for (int i = 0; i < documentsCount (); ++i)
      document (i)->setEditorFont (fontName);
}

// Update syntax HL for the current document
void QBEditor::slotUpdateHL ()
{
    editor->updateHL ();
}

// Current document number (-1 when no document)
int QBEditor::curDocNumber ()
{
    return documentSelector->currentIndex ();
}
