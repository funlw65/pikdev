#include "ActionsCollection.h"


ActionsCollection::ActionsCollection ()
{
}

QAction *ActionsCollection::addAction (const QString& name, const QString& iconname,
				       const QString& text,  const QKeySequence& shortcut,
				       QObject *parent)
{
    QMap<QString, QAction *>::iterator i = actions.find (name);
    QAction *a = 0;
    if (i == actions.end ())
      {
        a = new QAction (QIcon (iconname), text, parent);
        a->setShortcut (shortcut);
        actions[name] = a;
      }
    return a;
}

QAction *ActionsCollection::addAction (const QString& name, QAction *a)
{
    actions[name] = a;
    return a;
}

QAction *ActionsCollection::act (const QString& s) const
{
    QMap<QString, QAction *>::const_iterator i = actions.find (s);
    return (i == actions.end ()) ? 0 : *i;
}

void ActionsCollection::connect (const QString& name, QObject *dest, const char *slot)
{
    QAction *a = act (name);
    if (a)
      QObject::connect (a, SIGNAL (triggered ()), dest, slot);
}

