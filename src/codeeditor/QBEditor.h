/***************************************************************************
    begin                : 2013
    copyright            : (C)  2013 by Alain Gibaud
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

#ifndef QBEDITOR_H
#define QBEDITOR_H

#include <QAction>
#include <QDir>
#include <QIcon>
#include <QKeySequence>
#include <QLabel>
#include <QMap>
#include <QMenu>
#include <QObject>
#include <QSettings>
#include <QSpinBox>
#include <QStringList>
#include <QVector>

#include "QBEditorDocument.h"
#include "QBEditorSearchDialog.h"


class QBEditor : public QFrame
{
    Q_OBJECT
    CodeEditor *editor;
    QLabel *statusLabel;
    QSpinBox *lineSelector;
    QBEditorSearchDialog *searchDialog;
    QMap<QString, QAction*> actions;
    QComboBox *documentSelector;
    int lastDocIndex;
    static int fontSize_;
    static QString fontName_;
    // the followings are part of the editor configuration
    // true is path must be displayed in doc selector
    bool displayPaths;
    // default directory for file dialogs
    QDir defaultDir;

public:
    QBEditor (QWidget *w = 0);
    ~QBEditor ();
    // predefined actions framework
    void setupActions ();
    QAction *act (const QString&);
    QAction *addAction (const QString& name, QAction *a);
    QAction *addAction (const QString& name, const QIcon& icon, const QString& text,
                        const QKeySequence& shortcut, const char *, QObject *parent);
    // change current document, and update relations with the CodeEditor
    void setDocument (QBEditorDocument *document);
    // number of loaded documents
    int documentsCount ();
    // find document by index
    QBEditorDocument *document (int k);
    // find index for doc (-1 is not exists)
    int documentIndex (QBEditorDocument *doc);
    // find current document
    QBEditorDocument *currentDocument ();
    // find document by URL
    QBEditorDocument *document (const QUrl& u);
    // find index of a document by URL (-1 == not found)
    int documentIndex (const QUrl& u);
    // switch to specified document, and returns a pointer to it
    QBEditorDocument *switchToDocument (const QUrl& u);
    // load file, or reload it if it is already loaded
    // return the corresponding document
    QBEditorDocument *loadReloadFile (const QUrl& url);
    // make current the specified document, or create it,  if it is not currently open
    QBEditorDocument *switchOrLoad (const QUrl& url);
    // return the list of all opened files
    QStringList getAllFileNames ();
    // return the name to be displayed in the doc selector, according to displayPaths
    QString displayName (QBEditorDocument *doc);
    // save the current settings using the "settings" object
    void saveSettings (QSettings& settings);
    // restore the settings
    void restoreSettings (QSettings& settings);
    // manage text auto-alignment when typing
    void setAutoAlign (bool align);
    bool autoAlign ();
    // font management
    void setEditorFontSize (int sz);
    static int editorFontSize ();
    void setEditorFont (const QString& fontName);
    static QString editorFont ();
    // current document number = index of the current item in the document selector
    int curDocNumber ();

public slots:
    void slotNew ();
    void slotLoadFile ();
    void slotSaveFileAs ();
    void slotSaveFile ();
    void slotSaveAllFiles ();
    void slotToggleSearch ();
    void slotShowBookmarksMenu ();
    void slotUpdateStatusLine ();
    void slotGotoBookmark ();
    void slotFlashCurrentLine ();
    void slotFlashLine (int i);
    void slotUndo ();
    void slotRedo ();
    void slotCut ();
    void slotCopy ();
    void slotPaste ();
    void slotGotoLine ();
    void slotStopGotoLine ();
    void slotSelectDocument (int);
    void slotCloseCurrentDocument ();
    void slotCloseDocument (int index);
    void slotCloseDocument (const QUrl& url);
    void slotCloseAllDocuments ();
    void slotDeleteDocument (QBEditorDocument *doc);
    void slotToggleBookmarks ();
    void slotToggleLineNumbers ();
    void slotCommentBlock ();
    void slotUnCommentBlock ();
    void slotAboutToClose ();
    void slotIndent ();
    void slotLoadReloadFile (const QString& filename);
    void slotUpdateDocumentSelector (int k);
    void slotToggleShowPaths ();
    void slotZoomIn ();
    void slotZoomOut ();
    void slotUpdateHL ();

signals:
    void sigMustDeleteDocument (QBEditorDocument *);
};

#endif // QBEDITOR_H
