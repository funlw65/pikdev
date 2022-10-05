#ifndef P18HIGHLIGHTER_H
#define P18HIGHLIGHTER_H

QT_BEGIN_NAMESPACE
#include <QTextDocument>
QT_END_NAMESPACE

#include "codeeditor/HighlighterBase.h"


class P18Highlighter : public HighlighterBase
{
    Q_OBJECT

public:
    P18Highlighter (QTextDocument *parent = 0);

protected:
    void highlightBlock (const QString& text);

private:
    QVector<HLRule> HLRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat cppFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat fileNameFormat;
    QTextCharFormat placedSuffixFormat;
};

#endif // P18HIGHLIGHTER_H
