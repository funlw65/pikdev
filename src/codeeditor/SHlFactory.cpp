#include "CHighlighter.h"
#include "P18Highlighter.h"
#include "SHlFactory.h"


SHlFactory *SHlFactory::myself = 0;

SHlFactory::SHlFactory ()
{
    instances += new P18Highlighter;
    instances += new CHighlighter;
    QSettings settings;
    restoreSettings (settings);
}

// this is a static method
SHlFactory *SHlFactory::factory ()
{
    if (myself == 0)
      myself = new SHlFactory;
    return myself;
}

// this is a static method
HighlighterBase *SHlFactory::getSHl (const QString& filename)
{
    foreach (HighlighterBase *h, factory ()->instances)
      {
        if (h->isRelevant (filename))
	  return h;
      }
    return 0;
}

QStringList SHlFactory::getSHlNames ()
{
    QStringList r;
    foreach (HighlighterBase *h, factory ()->instances)
      {
        r << h->language ();
      }
    return r;
}

QList <HighlighterBase *> SHlFactory::HLs ()
{
    return factory ()->instances;
}

void SHlFactory::saveSettings (QSettings &settings)
{
    settings.beginGroup ("SyntaxHL");

    // allows to check whether the language is new
    settings.setValue ("LanguageNames", getSHlNames ());
    foreach (HighlighterBase *h, instances)
      {
        settings.beginGroup (h->language ());

        settings.setValue ("RuleNames", h->ruleNames ());
        QVector<HLRule>& R = h->rules ();
        foreach (const HLRule& rule, R)
	  {
            QString n = rule.name ();
            settings.setValue (n + "/color", rule.color ().name ());
            settings.setValue (n + "/style", rule.style ());
	  }

        settings.endGroup ();
      }

    settings.endGroup ();
}

void SHlFactory::restoreSettings (QSettings& settings)
{
    settings.beginGroup ("SyntaxHL");
    QStringList languages = settings.value ("LanguageNames", QStringList ()).toStringList ();

    foreach (HighlighterBase *h, instances)
      {
        // prevents from erasing new language
        if (languages.indexOf (h->language ()) == -1)
	  continue;

        settings.beginGroup (h->language ());

        QStringList rulenames = settings.value ("RuleNames", QStringList ()).toStringList ();

        QVector<HLRule>& R = h->rules ();
        QVector<HLRule>::iterator i;
        for (i = R.begin (); i != R.end (); ++i)
	  {
            QString n = (*i).name ();
            // prevents from erasing built-in new rules
            if (rulenames.indexOf (n) == -1)
	      continue;

            QString colorname = settings.value (n + "/color", "#000000").toString ();
            (*i).setColor (QColor (colorname));
            (*i).setStyle (settings.value (n + "/style", "").toString ());
	  }

        settings.endGroup ();
      }

    settings.endGroup ();
    settings.sync ();
}
