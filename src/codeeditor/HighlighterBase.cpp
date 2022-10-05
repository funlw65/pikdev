#include <iostream>
using namespace std;

#include "HighlighterBase.h"


QColor HLRule::color () const
{
    return format_.foreground ().color ();
}

void HLRule::setColor (const QColor& color)
{
    format_.setForeground (color);
}

QString HLRule::style () const
{
    return style_;
}

void HLRule::setStyle (const QString& style)
{
    style_ = style; // this information is redundant for convenience
    format_.setFontItalic (style.indexOf ("i") != -1);
    format_.setFontWeight ((style.indexOf ("b") != -1) ? QFont::Bold : QFont::Normal);
}

QString HLRule::name () const
{
    return name_;
}

HighlighterBase::HighlighterBase (QTextDocument *parent) : QSyntaxHighlighter (parent)
{
    setTabLength (3);
}

QString HighlighterBase::language ()
{
    return language_;
}

void HighlighterBase::setLanguage (const QString& lang, const QStringList& exts)
{
    language_ = lang;
    extensions_ = exts;
}

bool HighlighterBase::isRelevant (const QString& filename)
{
    QString ext = filename.section (".", -1);
    if (ext == "")
      return false;
    return extensions_.indexOf (ext) != -1;
}

void HighlighterBase::addRule (const QString& name, const QString& rexp,
			       const QColor& color, const QString& style)
{
    HLRule rule;
    rule.setStyle (style);
    rule.setColor (color);

    rule.patterns_ += QRegExp (rexp);
    rule.name_ = name;

    HLRules_ += rule;
}

void HighlighterBase::addRules (const QString& name, const QStringList& rexps,
				const QColor& color, const QString& style)
{
    HLRule rule;
    rule.setStyle (style);
    rule.setColor (color);
    rule.name_ = name;

    for (QStringList::const_iterator i = rexps.begin (); i != rexps.end (); ++i)
      rule.patterns_ += QRegExp (*i);

    HLRules_ += rule;
}

void HighlighterBase::setCommentPattern (const QString& c1, const QString& c2)
{
    comm1 = c1;
    comm2 = c2;
}

const QString& HighlighterBase::commentStart ()
{
    return comm1;
}

const QString& HighlighterBase::commentEnd ()
{
    return comm2;
}

int HighlighterBase::tabLength ()
{
    return tabL;
}

void HighlighterBase::setTabLength (int N)
{
    tabL = N;
}

const QStringList HighlighterBase::ruleNames ()
{
    QStringList names;
    const QVector<HLRule>& R = rules ();
    foreach (const HLRule& rule, R)
      {
        names += rule.name ();
      }
    return names;
}
