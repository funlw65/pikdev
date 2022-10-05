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

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <vector>
using namespace std;

#include <QMouseEvent>
#include <QObject>
#include <QPaintEvent>
#include <QPlainTextEdit>
#include <QRect>
#include <QResizeEvent>
#include <QSize>
#include <QTextBlock>
#include <QTextBlockUserData>
#include <QTextCursor>
#include <QWidget>

#include "HighlighterBase.h"


class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor (QWidget *parent = nullptr);
    void lineNumberAreaPaintEvent (QPaintEvent *event);
    bool lineNumberAreaClickEvent (QMouseEvent *event);
    int lineNumberAreaWidth ();
    int lineNumberHeight ();

    // enable/disable line numbering
    void setLineNumbersEnabled (bool e);
    // enable/disable boomarks
    void setBookmarksEnabled (bool e);
    // true if line numbers enabled
    bool lineNumbersEnabled () { return lineNumbers; }
    // true if bookmarks enabled
    bool bookmarksEnabled () { return bookmarks; }
    // true if bookmark set for this line/block
    bool isMarked (const QTextBlock& block);
    // change bookmarking status of this block
    void toggleMark (QTextBlock& block);
    // true if text must be auto-aligned when typing
    bool autoAlignText () { return autoAlign; }
    void setAutoAlignText (bool align) { autoAlign = align; }

    // returns the list of bookmarked lines, as line numbers
    QVector<int> markedLines ();
    // change the syntax HL
    void setSyntaxHL (HighlighterBase *hl);
    // language used for HL
    QString getLanguage ();
    // return the line #i of the text
    QString getLineText (int i);
    // return the position of cursor in l and c
    void getCursorPosition (int& l, int& c);
    // change the cursor position
    void setCursorPosition (int l, int c);
    // return the current file name, if any
    QString getFileName ();
    // helper for line flashing
    void highlightCurrentLine_ (const QColor& lineColor);
    // comment the current selection
    void comment ();
    // un-comment the current selection
    void unComment ();
    // set tab width to N chars
    void setTab (int N);
    // experimental
    int openMarkPos (QTextCursor&);

protected:
    void resizeEvent (QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth (int newBlockCount);
    void updateLineNumberArea (const QRect&, int);

public slots:
    void highlightCurrentLine ();
    void slotFlashCurrentLine ();
    void slotSearch (const QString& toBeFound, bool backward, bool caseSensitive);
    void slotReplace (const QString& newString);
    void slotSearchReplace (const QString& oldString, const QString& newString,
			    bool backward, bool caseSensitive);
    void slotGotoLine (int line);
    void slotGotoPos (int line, int column);
    void slotLineAdded (int newcount);
    void updateHL ();

signals:
    void statusChanged ();

private:
    QWidget *lineNumberArea;
    bool lineNumbers, bookmarks, autoAlign;
    HighlighterBase *syntaxHL;
    QString filename;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea (CodeEditor *editor) : QWidget (editor)
    {
        codeEditor = editor;
    }

    QSize sizeHint () const
    {
        return QSize (codeEditor->lineNumberAreaWidth (), 0);
    }

    virtual void mousePressEvent (QMouseEvent *e);

protected:
    void paintEvent (QPaintEvent *event)
    {
        codeEditor->lineNumberAreaPaintEvent (event);
    }

private:
    CodeEditor *codeEditor;
};

class LineData : public QTextBlockUserData
{
public:
    LineData () : bookmark (0) { }

    int bookmark;
};

#endif
