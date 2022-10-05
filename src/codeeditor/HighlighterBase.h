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

#ifndef HIGHLIGHTERBASE_H
#define HIGHLIGHTERBASE_H

#include <QColor>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>


class HLRule
{
public:
    QString name_;
    QList<QRegExp> patterns_;
    QTextCharFormat format_;
    QString style_;

    QColor color () const;
    void setColor (const QColor& color);
    QString style () const;
    void setStyle (const QString& style);
    QString name () const;
    // no setter - rule name cannot change
};

class HighlighterBase : public QSyntaxHighlighter
{
protected:
    QStringList extensions_;
    QString language_;
    QString comm1, comm2;
    int tabL;
    QString indenter_;
    QVector<HLRule> HLRules_;

public:
    QVector<HLRule>& rules () { return HLRules_; }
    HighlighterBase (const HighlighterBase& hl);
    HighlighterBase (QTextDocument *parent);
    QString language ();
    void setLanguage (const QString& lang, const QStringList& exts);
    bool isRelevant (const QString& filename);
    void addRule (const QString& name, const QString& rexp, const QColor& color,
		  const QString& style = "");
    void addRules (const QString& name, const QStringList& rexps, const QColor& color,
		   const QString& style = "");
    void setCommentPattern (const QString& start, const QString& end = "");
    const QString& commentStart ();
    const QString& commentEnd ();
    int tabLength ();
    void setTabLength (int);
    void setIndenter (const QString& fullcommand) { indenter_ = fullcommand; }
    const QString& indenter () { return indenter_; }
    const QStringList ruleNames ();
};

#endif // HIGHLIGHTERBASE_H
