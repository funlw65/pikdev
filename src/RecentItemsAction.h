#ifndef RECENTITEMSACTION_H
#define RECENTITEMSACTION_H

#include <QAction>
#include <QIcon>
#include <QList>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QStringList>


class RecentItemsAction : public QAction
{
    Q_OBJECT
    QStringList names;
    QList<QRegExp> regexps;
    QList<QIcon> icons;
    int max_size;

public:
    explicit RecentItemsAction (const QString&, QObject *parent = 0);
    void addEntry (const QString& name);
    void addIconRule (const QRegExp& reg, const QIcon& icon);
    const QStringList& getNames () const;
    void setMaxSize (int size);
    int maxSize ();
    void loadItems (const QString& collectiveName, QSettings& settings);
    void saveItems (const QString& collectiveName, QSettings& settings);

signals:
    void itemSelected (const QString& name);

public slots:
    void slotAboutToShow ();
    void slotItemSelected (QAction *);
};

#endif // RECENTITEMSACTION_H
