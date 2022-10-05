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

#ifndef QBEDITORSEARCHDIALOG_H
#define QBEDITORSEARCHDIALOG_H

#include <QComboBox>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QWidget>


class QBEditorSearchDialog : public QFrame
{
    Q_OBJECT
    QPushButton *search, *replace, *search_replace;
    QLineEdit *edSearch, *edReplace;
    QComboBox *searchDirection, *caseSensitive;

public:
    QBEditorSearchDialog (QWidget *parent);

private slots:
    void slotInvisible ();
    void slotSearch ();
    void slotSearchReplace ();
    void slotReplace ();

public slots:
    void slotSetVisibility (bool);

signals:
    void sigSearch (const QString& toBeFound, bool backward, bool caseSensitive);
    void sigSearchReplace (const QString& toBeFound, const QString& newString,
			   bool backward, bool caseSensitive);
    void sigReplace (const QString& newString);
    void sigHide ();
};

#endif // QBEDITORSEARCHDIALOG_H
