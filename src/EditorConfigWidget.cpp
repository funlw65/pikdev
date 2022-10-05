#include <iostream>
using namespace std;

#include <QColorDialog>
#include <QGroupBox>
#include <QPushButton>

#include "codeeditor/QBEditor.h"
#include "codeeditor/SHlFactory.h"
#include "EditorConfigWidget.h"
#include "PikMain.h"


EditorConfigWidget::EditorConfigWidget (QWidget *parent) : ConfigWidget(parent)
{
    QBEditor *ed = PikMain::mainWindow ()->editor ();

    QGroupBox *gen = new QGroupBox (tr ("General"), this);
    addWidget (gen);
    QGridLayout *genl = new QGridLayout (gen);
    genl->setMargin (15);
    autoAlignText = new QCheckBox (tr ("Text Auto alignment"), gen);
    genl->addWidget (autoAlignText, 0, 0);
    autoAlignText->setChecked (ed->autoAlign ());

    QLabel *labfont = new QLabel (tr ("Font"));
    labfont->setAlignment (Qt::AlignRight | Qt::AlignVCenter);
    genl->addWidget (labfont, 0, 1);

    fontSelector = new QFontComboBox (gen);
    int k = fontSelector->findText (ed->editorFont ());
    if (k != -1)
      fontSelector->setCurrentIndex (k);
    genl->addWidget (fontSelector, 0, 2);

    QLabel *labsize = new QLabel (tr ("Size"));
    labsize->setAlignment (Qt::AlignRight | Qt::AlignVCenter);
    genl->addWidget (labsize, 0, 3);

    fontSize = new QSpinBox (gen);
    genl->addWidget (fontSize, 0, 4);
    fontSize->setValue (ed->editorFontSize ());

    QGroupBox *lang = new QGroupBox (tr ("Language specific highlighting"), this);
    addWidget (lang);
    QGridLayout *langL = new QGridLayout (lang);
    langL->setMargin (15);
    QComboBox *langNames = new QComboBox (lang);
    langL->addWidget (langNames, 0, 0);
    langNames->addItems (SHlFactory::factory ()->getSHlNames ());

    // get HL list
    QList<HighlighterBase *> HLs = SHlFactory::factory ()->HLs ();
    QLabel *name;
    QPushButton *color;
    QCheckBox *bold, *italic;
    QWidget *page;
    stack = new QStackedLayout;
    langL->addLayout (stack, 1, 0);

    for (int i = 0; i < HLs.count (); ++i)
      {
        HighlighterBase *hl = HLs[i];
        QVector<HLRule>& rules = hl->rules ();
        page = new QWidget (lang);
        stack->addWidget (page);
        QGridLayout *pageL = new QGridLayout (page);

        for (int r = 0; r < rules.count (); ++r)
	  {
	    name = new QLabel ("<b>" + rules[r].name () + "</b>", lang);
	    pageL->addWidget (name, r, 0, 1, 1);

	    QColor c = rules[r].color ();
	    color = new QPushButton (c.name (), lang);
	    connect (color, SIGNAL (clicked ()), this, SLOT (selectColor ()));

	    QPalette p = color->palette ();
	    p.setColor (QPalette::Button, c);
	    p.setColor (QPalette::ButtonText, QColor (0xFF, 0xFF, 0xFF));
	    color->setPalette (p);
	    pageL->addWidget (color, r, 1, 1, 1);

	    QString style = rules[r].style ();
	    bold = new QCheckBox (tr ("Bold"), lang);
	    bold->setChecked (style.contains ('b'));
	    pageL->addWidget (bold, r, 2, 1, 1);

	    italic = new QCheckBox (tr ("Italic"), lang);
	    italic->setChecked (style.contains ('i'));
	    pageL->addWidget (italic, r, 3, 1, 1);
         }
      }

    connect (langNames, SIGNAL (activated (int)), stack, SLOT (setCurrentIndex (int)));
}

void EditorConfigWidget::selectColor ()
{
    QPushButton *b = dynamic_cast<QPushButton *>(sender ());
    if (b)
      {
        QPalette p = b->palette ();
        QColor c = p.color (QPalette::Button);
        c  = QColorDialog::getColor (c, this);
        if (c.isValid ())
	  {
            p.setColor (QPalette::Button, c);
            b->setPalette (p);
            b->setText (c.name ());
	  }
      }
}

EditorConfigWidget::~EditorConfigWidget ()
{
}

void EditorConfigWidget::doApply ()
{
    QBEditor *ed = PikMain::mainWindow ()->editor ();
    // global settings
    ed->setAutoAlign (autoAlignText->isChecked ());
    ed->setEditorFontSize (fontSize->value ());
    ed->setEditorFont (fontSelector->currentFont ().family ());

    // syntax HL settings are harder to parse ..
    QList<HighlighterBase *> HLs = SHlFactory::factory ()->HLs ();
    int hlnumber = stack->currentIndex ();
    HighlighterBase *hl = HLs[hlnumber];
    QVector<HLRule>& rules = hl->rules ();

    // find layoutmanager for this page
    QWidget *w = stack->currentWidget ();
    QGridLayout *wL = dynamic_cast<QGridLayout *>(w->layout ());
    if (wL && (wL->rowCount () == rules.count ())) // to be 120% sure
      {
	for (int i = 0; i < wL->rowCount (); ++i)
	  {
	    // process color
	    QWidget *w = wL->itemAtPosition (i, 1)->widget ();
	    QPushButton *b = (QPushButton *) w;
	    QColor col = b->palette ().color (QPalette::Button);
	    rules[i].setColor (col);

	    // process style
	    QString style;
	    w = wL->itemAtPosition (i, 2)->widget ();
	    QCheckBox *cb = (QCheckBox *) w;
	    if (cb->isChecked ())
	      style += "b";
	    w = wL->itemAtPosition (i, 3)->widget ();
	    cb = (QCheckBox *) w;
	    if (cb->isChecked ())
	      style += "i";
	    rules[i].setStyle (style);
	  }
      }

    // TODO : force an update of the current document
    ed->slotUpdateHL ();

    QSettings settings;
    ed->saveSettings (settings);
    SHlFactory::factory ()->saveSettings (settings);
}

QString EditorConfigWidget::configName ()
{
    return "Editor";
}

QString EditorConfigWidget::configTitle ()
{
    return "Editor configuration";
}

QPixmap EditorConfigWidget::icon ()
{
    return QPixmap (":/src/icons/32/pikdev_config_editor.png");
}
