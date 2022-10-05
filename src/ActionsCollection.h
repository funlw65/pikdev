#ifndef ACTIONSCOLLECTION_H
#define ACTIONSCOLLECTION_H

#include <QAction>
#include <QKeySequence>
#include <QMap>
#include <QObject>
#include <QSettings>
#include <QString>


class ActionsCollection : public QObject
{
    Q_OBJECT
    QMap<QString, QAction *> actions;

public:
    ActionsCollection ();
    QAction *addAction (const QString& name, const QString& iconName, const QString& menuText,
			const QKeySequence& shortcut, QObject *parent);
    QAction *addAction (const QString& name, QAction *);
    QAction *act (const QString& name) const;
    void connect (const QString& actionname, QObject *target, const char *slot);
};

#endif // ACTIONSCOLLECTION_H
