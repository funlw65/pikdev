#include <algorithm>
#include <iostream>
using namespace std;

#include <QApplication>
#include <QtGui>

#include "CodeEditor.h"
#include "QBEditorDocument.h"
#include "SHlFactory.h"


// enable/disable line numbering
void CodeEditor::setLineNumbersEnabled (bool e)
{
    lineNumbers = e;
    updateLineNumberAreaWidth (0);
}

// enable/disable boomarks
void CodeEditor::setBookmarksEnabled (bool e)
{
    bookmarks = e;
    updateLineNumberAreaWidth (0);
}

CodeEditor::CodeEditor (QWidget *parent) : QPlainTextEdit (parent), syntaxHL (0)
{
    lineNumberArea = new LineNumberArea (this);
    setSyntaxHL (SHlFactory::factory ()->getSHl (""));

    setLineNumbersEnabled (true);
    setBookmarksEnabled (true);

    connect (this, SIGNAL (blockCountChanged (int)), this,
	     SLOT (updateLineNumberAreaWidth (int)));
    connect (this, SIGNAL (updateRequest (QRect, int)), this,
	     SLOT (updateLineNumberArea (QRect, int)));
    connect (this, SIGNAL (cursorPositionChanged ()), this, SLOT (highlightCurrentLine ()));
    connect (this, SIGNAL (cursorPositionChanged ()), this, SIGNAL (statusChanged ()));
    connect (this, SIGNAL (blockCountChanged (int)), this, SLOT (slotLineAdded (int)));

    updateLineNumberAreaWidth (0);
    highlightCurrentLine ();
    setReadOnly (true);
}

void CodeEditor::slotLineAdded (__attribute__ ((unused)) int newcount)
{
    if (! autoAlign)
      return;

    QTextCursor c = textCursor ();
    if (c.columnNumber () == 0)
      {
        // retrieve previous row
        QTextCursor prev (c);
        prev.movePosition (QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, 1);
        QTextBlock pblock = prev.block ();
        if (pblock.isValid ())
	  {
            QString pline = pblock.text ();
            // extract line header
            QString header;
            for (int i = 0; i < pline.size (); ++i)
	      {
                QChar h = pline[i];
                if (h == QChar (' ') || h == QChar ('\t'))
		  header += h;
                else
		  break;
	      }
            c.insertText (header);
	  }
      }
}

// try to find the {
// WORK IN PROGRESS - this function is not used yet
int CodeEditor::openMarkPos (QTextCursor& curs)
{
    int pos;
    for (pos = curs.position (); pos > 0; --pos)
      {
        // TODO, to finish
        curs.setPosition (pos - 1, QTextCursor::KeepAnchor);
        QString car = curs.selectedText ();
        curs.setPosition (pos - 1, QTextCursor::MoveAnchor);
        cout << car.toStdString () << flush;
      }
    return 0;
}

int CodeEditor::lineNumberAreaWidth ()
{
    int digits = 1;
    int max = qMax (1, blockCount ());
    while (max >= 10)
      {
        max /= 10;
        ++digits;
      }

    int space = 3;
    if (lineNumbersEnabled ())
      space += fontMetrics ().horizontalAdvance (QLatin1Char ('9')) * digits;
    if (bookmarksEnabled ())
      space += fontMetrics ().horizontalAdvance (QLatin1Char ('W'));

    return space;
}

int CodeEditor::lineNumberHeight ()
{
    return fontMetrics ().height ();
}

void CodeEditor::updateLineNumberAreaWidth (__attribute__ ((unused)) int newBlockCount)
{
    setViewportMargins (lineNumberAreaWidth (), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea (const QRect& rect, int dy)
{
    if (dy)
      lineNumberArea->scroll (0, dy);
    else
      lineNumberArea->update (0, rect.y (), lineNumberArea->width (), rect.height ());

    if (rect.contains (viewport ()->rect ()))
      updateLineNumberAreaWidth (0);
}

void CodeEditor::resizeEvent (QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent (e);

    QRect cr = contentsRect ();
    lineNumberArea->setGeometry (QRect (cr.left (), cr.top (),
					lineNumberAreaWidth (), cr.height ()));
}

void CodeEditor::highlightCurrentLine ()
{
    highlightCurrentLine_ (QColor (Qt::yellow).lighter (160));
}

void CodeEditor::highlightCurrentLine_ (__attribute__ ((unused)) const QColor& lineColor)
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (! isReadOnly ())
      {
        QTextEdit::ExtraSelection selection;
	QPalette p = this->palette ();
	QColor c = p.color (QPalette::Text);

	if (c.lightness () < 100)
	  selection.format.setBackground (QColor (Qt::yellow).lighter (160));	
	else
	  selection.format.setBackground (QColor (Qt::gray).darker (160));
	
        selection.format.setProperty (QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor ();
        selection.cursor.clearSelection ();
        extraSelections.append (selection);
      }

    setExtraSelections (extraSelections);
}

void CodeEditor::slotFlashCurrentLine ()
{
    highlightCurrentLine_ (QColor (0xFF, 0x80, 0x00));

    QTimer::singleShot (250, this, SLOT (highlightCurrentLine ()));
}

void CodeEditor::lineNumberAreaPaintEvent (QPaintEvent *event)
{
    QPainter painter (lineNumberArea);
    painter.fillRect (event->rect (), Qt::lightGray);

    QTextBlock block = firstVisibleBlock ();
    int blockNumber = block.blockNumber ();
    int top = (int) blockBoundingGeometry (block).translated (contentOffset ()).top ();
    int bottom = top + (int) blockBoundingRect (block).height ();
    int lineH = lineNumberHeight ();
    int bookmarkBottom = top + lineH;

    while (block.isValid () && top <= event->rect ().bottom ())
      {
        if (block.isVisible () && bottom >= event->rect ().top ())
	  {
            QString number = QString::number (blockNumber + 1);
            painter.setPen (Qt::black);

            int bookmarkWidth = (bookmarksEnabled ()
				 ? fontMetrics ().horizontalAdvance (QLatin1Char ('W')) : 0);

            if (lineNumbersEnabled ())
	      {
                painter.drawText (bookmarkWidth, top, lineNumberArea->width () - bookmarkWidth,
				  fontMetrics ().height (), Qt::AlignRight, number);
	      }

            if (bookmarksEnabled ())
	      {
                if (isMarked (block))
		  {
                    QPolygon triangle;
                    QPoint p1 (bookmarkWidth, (top + bookmarkBottom) / 2);
		    QPoint p2 (3, top + 2), p3 (3, bookmarkBottom - 2);
                    triangle += p1;
                    triangle += p2;
                    triangle += p3;

                    painter.setRenderHint (QPainter::Antialiasing, true);
                    painter.setPen (QColor (0, 0xC0, 0));

                    QPointF pf1 (p3), pf2 (p1);
                    QLinearGradient lgrad (pf1, pf2);
                    lgrad.setColorAt (0., QColor (0, 0xC0, 0));
                    lgrad.setColorAt (1., QColor (0xAD, 0xED, 0x80));
                    QBrush gradbrush (lgrad);
                    painter.setBrush (gradbrush);

                    painter.drawConvexPolygon (triangle);
		  }
	      }
	  }

        block = block.next ();
        top = bottom;
        bottom = top + (int) blockBoundingRect (block).height ();
        bookmarkBottom = top + lineH;
        ++blockNumber;
      }
}

bool CodeEditor::lineNumberAreaClickEvent (QMouseEvent *event)
{
    if (! bookmarksEnabled ())
      return false;

    QTextBlock block = firstVisibleBlock ();

    int top = (int) blockBoundingGeometry (block).translated (contentOffset ()).top ();
    int bottom = top + (int) blockBoundingRect (block).height ();

    int y = event->y ();

    while (block.isValid () && bottom < y)
      {
        block = block.next ();
        top = bottom;
        bottom = top + (int) blockBoundingRect (block).height ();
      }
    if (y >= top && y < bottom && block.isValid ())
      {
        toggleMark (block);
        update ();
      }
    return true;
}

bool CodeEditor::isMarked (const QTextBlock& block)
{
    LineData *ud = dynamic_cast<LineData *>(block.userData ());
    return ud != 0 && ud->bookmark != 0;
}

void CodeEditor::toggleMark (QTextBlock& block)
{
    LineData *ud = dynamic_cast<LineData *>(block.userData ());
    if (ud == 0)
      block.setUserData (ud = new LineData);
    ud->bookmark = ! ud->bookmark;
}

QVector<int> CodeEditor::markedLines ()
{
    QVector<int> linesMarked;

    QTextBlock b = document ()->firstBlock ();
    for (; b.isValid (); b = b.next ())
      {
        if (isMarked (b))
	  linesMarked.push_back (b.blockNumber ());
      }
    return linesMarked;
}

void CodeEditor::setSyntaxHL (HighlighterBase *hl)
{
    // disconnect the old document
    if (syntaxHL)
      syntaxHL->setDocument (0);
    if (hl)
      {
        hl->setDocument (document ());
        setTab (hl->tabLength ());
        emit statusChanged ();
      }
    syntaxHL = hl;
}

QString CodeEditor::getLanguage ()
{
    return syntaxHL ? syntaxHL->language () : tr ("");
}

QString CodeEditor::getLineText (int i)
{
    return document ()->findBlockByNumber (i).text ();
}

void CodeEditor::getCursorPosition (int& l, int& c)
{
    QTextCursor cursor = textCursor ();
    l = cursor.blockNumber () + 1;
    c = cursor.columnNumber () + 1;
}

void CodeEditor::setCursorPosition (int l, int c)
{
    QTextCursor cursor = textCursor ();
    cursor.movePosition (QTextCursor::Start, QTextCursor::MoveAnchor, 1);
    cursor.movePosition (QTextCursor::NextBlock, QTextCursor::MoveAnchor, l - 1);
    cursor.movePosition (QTextCursor::NextCharacter, QTextCursor::MoveAnchor, c - 1);
    setTextCursor (cursor);
}

QString CodeEditor::getFileName ()
{
    return filename;
}

void CodeEditor::slotSearch (const QString& w, bool back, bool cs)
{
    int flags = 0;

    if (back)
      flags |= QTextDocument::FindBackward;
    if (cs)
      flags |= QTextDocument::FindCaseSensitively;

    if (! find (w, (QTextDocument::FindFlags) flags))
      {
        moveCursor (back ? QTextCursor::End : QTextCursor::Start);
        QApplication::beep ();
        find (w, (QTextDocument::FindFlags) flags);
      }
}

void CodeEditor::slotReplace (const QString& newText)
{
    if (textCursor ().hasSelection ())
      textCursor ().insertText (newText);
}

void CodeEditor::slotSearchReplace (const QString& oldString, const QString& newString,
				    bool back, bool cs)
{
    slotSearch (oldString, back, cs);
    slotReplace (newString); // note: no selection = no replace
}

void CodeEditor::slotGotoLine (int numline)
{
    QTextCursor cursor (document ());
    cursor.movePosition (QTextCursor::NextBlock, QTextCursor::MoveAnchor, numline - 1);
    setTextCursor (cursor);
    setFocus ();
}

void CodeEditor::slotGotoPos (int numline, int numcol)
{
    QBEditorDocument *doc = (QBEditorDocument *) document ();
    QTextCursor cursor (doc);
    cursor.movePosition (QTextCursor::NextBlock, QTextCursor::MoveAnchor, numline - 1);
    cursor.movePosition (QTextCursor::StartOfBlock, QTextCursor::MoveAnchor, 1);
    cursor.movePosition (QTextCursor::NextCharacter, QTextCursor::MoveAnchor, numcol - 1);

    doc->prevLine = numline;
    doc->prevColumn = numcol;
    setTextCursor (cursor);
    highlightCurrentLine ();

    setFocus ();
}
/* Makes a comment with the current block
   If commentStart and commentEnd are both non empty, a C-like comment is generated
   If commentEnd == "", each line is commented out by adding commentStart to the start
   of each line */
void CodeEditor::comment ()
{
    QTextCursor cursor = textCursor ();
    if (cursor.hasSelection ())
      {
        int s = cursor.selectionStart (), e = cursor.selectionEnd ();
        cursor.clearSelection ();
        QString start = syntaxHL->commentStart ();
        QString end = syntaxHL->commentEnd ();

        if (end != "")
	  {
            cursor.setPosition (e);
            cursor.insertText (end);
            cursor.setPosition (s);
            cursor.insertText (start);
	  }
        else
	  {
            cursor.setPosition (e);
            // line must not be commented out when block end is a start of line
            if (cursor.atBlockStart ())
	      cursor.movePosition (QTextCursor::PreviousBlock);
            while (cursor.position () > s)
	      {
                cursor.movePosition (QTextCursor::StartOfBlock);
                cursor.insertText (start + " ");
                cursor.movePosition (QTextCursor::PreviousBlock);
	      }
	  }
        setTextCursor (cursor);
      }
}

void CodeEditor::unComment ()
{
    QTextCursor cursor = textCursor ();
    if (cursor.hasSelection ())
      {
        int s = cursor.selectionStart (), e = cursor.selectionEnd ();
        cursor.clearSelection ();
        QString start = syntaxHL->commentStart ();
        QString end = syntaxHL->commentEnd ();
        if (end == "")
	  {
            cursor.setPosition (e);
            QTextCursor com;
            QRegExp rex ("^[ \\t]*" + start + "[ \\t]*");
            com = document ()->find (rex, cursor, QTextDocument::FindBackward);
            while (! com.isNull () && com.position () >= s)
	      {
                cursor.setPosition (com.position ());
                com.removeSelectedText ();
                com = document ()->find (rex, cursor, QTextDocument::FindBackward);
	      }
	  }
        else
	  {
            cursor.setPosition (s);
            QTextCursor soc = document ()->find ("/*", cursor);
            if (! soc.isNull () && soc.position () < e)
	      {
                cursor.setPosition (e);
                QTextCursor eoc = document ()->find ("*/", cursor, QTextDocument::FindBackward);
                if (! eoc.isNull () && eoc.position () > s)
		  {
                    // everything is OK, remove found markers
                    eoc.removeSelectedText ();
                    soc.removeSelectedText ();
		  }
	      }
	  }
        setTextCursor (cursor);
      }
}

void CodeEditor::setTab (int N)
{
    int k = fontMetrics ().horizontalAdvance (QLatin1Char ('X')) * N;
    setTabStopDistance (k);
}

// force HL rules to be reparsed ans applied
void CodeEditor::updateHL ()
{
    setSyntaxHL (syntaxHL);
}

// LineNumberArea class
void LineNumberArea::mousePressEvent (QMouseEvent *e)
{
    if (! codeEditor->lineNumberAreaClickEvent (e))
      QWidget::mousePressEvent (e);
}
