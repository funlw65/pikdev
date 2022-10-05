#include <QGridLayout>
#include <QLabel>
#include <QtGui>

#include "QBEditorSearchDialog.h"


QBEditorSearchDialog::QBEditorSearchDialog (QWidget *parent) : QFrame (parent)
{
    QColor backg (0xA0, 0xA0, 0xA0);

    QPalette p = palette ();
    p.setColor (QPalette::Active, QPalette::Window, backg);

    setPalette (p);
    setAutoFillBackground (true);

    QGridLayout *lay = new QGridLayout (this);
    lay->setVerticalSpacing (0);
    QLabel *lbSearch = new QLabel ("Search :", this);
    lbSearch->setMargin (1);
    QLabel *lbReplace = new QLabel ("Replace by :", this);
    lbReplace->setMargin (1);

    edSearch = new QLineEdit (this);
    edReplace = new QLineEdit (this);

    QPushButton *close = new QPushButton (QIcon (":/src/codeeditor/icons/dialog-close.png"),
					  "", this);
    close->setFlat (true);
    p = close->palette ();
    p.setColor (QPalette::Active, QPalette::Button, backg);
    p.setColor (QPalette::Active, QPalette::ButtonText, QColor (0xFF, 0xFF, 0xFF));

    close->setPalette (p);
    close->setAutoFillBackground (true);
    close->setFixedSize (QSize (22, 22));
    connect (close, SIGNAL (clicked ()), this, SLOT (slotInvisible ()));

    search = new QPushButton (tr ("Search"), this);
    search->setMaximumWidth (search->sizeHint ().width () - 20);
    search->setFlat (true);
    search->setPalette (p);

    replace = new QPushButton (tr ("Replace"), this);
    replace->setFlat (true);
    replace->setPalette (p);
    replace->setMaximumWidth (replace->sizeHint ().width () - 20);

    search_replace = new QPushButton (tr ("Search+Replace"), this);
    search_replace->setFlat (true);
    search_replace->setPalette (p);
    search_replace->setMaximumWidth (search_replace->sizeHint ().width () - 20);

    searchDirection = new QComboBox (this);
    searchDirection->setToolTip (tr ("Select search direction"));
    searchDirection->insertItem (0, QIcon (":/src/codeeditor/icons/search-forward.png"), "");
    searchDirection->insertItem (1, QIcon (":/src/codeeditor/icons/search-backward.png"), "");
    searchDirection->setMaximumWidth (searchDirection->sizeHint ().width ());

    caseSensitive = new QComboBox (this);
    caseSensitive->setToolTip (tr ("Select case sensitivity"));
    caseSensitive->insertItem (0, QIcon (":/src/codeeditor/icons/NCS.png"), "");
    caseSensitive->insertItem (1, QIcon (":/src/codeeditor/icons/CS.png"), "");
    caseSensitive->setMaximumWidth (caseSensitive->sizeHint ().width ());

    lay->addWidget (lbSearch, 0, 0);
    lay->addWidget (lbReplace, 1, 0);
    lay->addWidget (edSearch, 0, 1);
    lay->addWidget (edReplace, 1, 1);
    lay->addWidget (close, 0, 5);
    lay->addWidget (search, 0, 2);
    lay->addWidget (replace, 1, 2);
    lay->addWidget (search_replace, 1, 3, 1, 2);
    lay->addWidget (searchDirection, 0, 3);
    lay->addWidget (caseSensitive, 0, 4);

    connect (search, SIGNAL (clicked ()), this, SLOT (slotSearch ()));
    connect (replace, SIGNAL (clicked ()), this, SLOT (slotReplace ()));
    connect (search_replace, SIGNAL (clicked ()), this, SLOT (slotSearchReplace ()));
}

void QBEditorSearchDialog::slotInvisible ()
{
    setVisible (false);
    emit sigHide ();
}

void QBEditorSearchDialog::slotSetVisibility (bool visible)
{
    setVisible (visible);
    if (visible)
      edSearch->setFocus ();
    else
      emit sigHide ();
}

void QBEditorSearchDialog::slotSearch ()
{
    bool cs = caseSensitive->currentIndex () == 1;
    bool back = searchDirection->currentIndex () == 1;
    emit sigSearch (edSearch->text (), back, cs);
}

void QBEditorSearchDialog::slotSearchReplace ()
{
    bool cs = caseSensitive->currentIndex () == 1;
    bool back = searchDirection->currentIndex () == 1;
    emit sigSearchReplace (edSearch->text (), edReplace->text (), back, cs);
}

void QBEditorSearchDialog::slotReplace ()
{
    emit sigReplace (edReplace->text ());
}
