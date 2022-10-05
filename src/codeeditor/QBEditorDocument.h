/***************************************************************************
    begin                : 2013
    copyright            : (C)  2013 by Alain Gibaud
    email                : alain.gibaud@univ-valenciennes.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QBEDITORDOCUMENT_H
#define QBEDITORDOCUMENT_H

#include <QString>
#include <QTextDocument>
#include <QUrl>

#include "CodeEditor.h"


class QBEditorDocument : public QTextDocument
{
    Q_OBJECT

public:
    QUrl url_;
    int currentLine, prevLine, prevColumn;
    CodeEditor *currentEditor;
    bool analyzed_, readonly_;

public:
    QBEditorDocument (QObject *parent, const QUrl& url);
    void setURL (const QUrl& url);
    void setEditor (CodeEditor *e);
    CodeEditor *editor ();
    QString fileName (); // file name
    QString path (); // path (i.e.: directories) only
    QString fileFullName (); // path + file name
    // load a new file, return an error message (empty message = OK)
    QString loadFile (const QUrl& url);
    // save a new file, return an error message (empty message = OK)
    QString saveFileAs (const QUrl& url);
    // save file, with current name , return an error message (empty message = OK)
    QString saveFile ();
    // url of this document
    const QUrl& url();
    // true when source code has been analysed
    bool analyzed ();
    // set analysed status
    void setAnalyzed (bool a = false);
    // get line number i
    QString getLineText (int i);
    // read only flag
    void setReadOnly (bool);
    bool isReadOnly ();
    // change default font size
    void setEditorFontSize (int sz);
    // change current font
    void setEditorFont (const QString& fname);

public slots:
    // set analysed status to false
    void slotNotAnalyzed ();
};

#endif // QBEDITORDOCUMENT_H
