/***************************************************************************
                    GlobalConfigWidget.cpp  -  description
                             -------------------
    begin                : Thu Feb 2 2006
    copyright            : (C) 2006 by Gibaud Alain
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

#include <iostream>
using namespace std;

#include <QFileDialog>
#include <QGroupBox>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QToolTip>

#include "Config.h"
#include "GlobalConfigWidget.h"
#include "XQPushButton.h"


GlobalConfigWidget::GlobalConfigWidget (PikMain *pikmain)
    : ConfigWidget (0), main_widget (pikmain)
{

    QGroupBox *programmersbox = new QGroupBox (tr ("Programmers"), this);
    addWidget (programmersbox);
    QGridLayout *Lprogrammersbox = new QGridLayout (programmersbox);
    Lprogrammersbox->setMargin (15);
    programmers = new QComboBox (programmersbox);
    Lprogrammersbox->addWidget (programmers, 0, 0);

    // get current programmer shortname
    QString shortname;
    Config::getCurrentProgrammer (shortname);

    // get programmers names
    QVector<ProgrammerBase *>& progs = main_widget->programmersList ();
    int cp = 0;
    QString path;
    for (int i = 0; i < (int) progs.size (); ++i)
      {
        programmers->addItem (progs[i]->longName ());
        if (progs[i]->shortName () == shortname)
	  cp = i;
      }
    // display current programmer
    programmers->setCurrentIndex (cp);

    programmersbox->setFixedHeight (programmersbox->sizeHint ().height ());

    QGroupBox *toolsbox = new QGroupBox (tr ("External tools"), this);
    addWidget (toolsbox);
    QGridLayout *Ltoolsbox = new QGridLayout (toolsbox);
    Ltoolsbox->setMargin (15);

    QStringList toolnames;
    Config::getToolNames (toolnames);
    QStringList::iterator t;
    int nb;

    QIcon iconset (":src/icons/22/pikdev_openfile.png");

    for (t = toolnames.begin (), nb = 0; t != toolnames.end (); ++t, ++nb)
      {
        QString path, info, URL;
        bool mand;
        Config::getToolEntry (*t, path, info, URL, mand);

        QLabel *lab = new QLabel (*t, toolsbox);
	Ltoolsbox->addWidget (lab, nb, 0);
        tool_names.push_back (lab);
        QLineEdit *p = new QLineEdit (path, toolsbox);
	Ltoolsbox->addWidget (p, nb, 1);
        tool_paths.push_back (p);

        QString tttext = info;
        tttext += tr ("\nDownload:  ");
        tttext += URL;
        p->setToolTip (tttext);

        XQPushButton *b = new XQPushButton (QString (), toolsbox, nb);
        Ltoolsbox->addWidget (b, nb, 2);
        connect (b, SIGNAL (clicked (int)), this, SLOT (chooseExecutable (int)));
        b->setIcon (iconset);

        Ltoolsbox->setRowStretch (nb, 0);
      }
    QWidget *spacer = new QWidget (this);
    Ltoolsbox->addWidget (spacer, nb, 2, 1, 3);
    Ltoolsbox->setRowStretch (nb, 1);
}

GlobalConfigWidget::~GlobalConfigWidget ()
{
}

// short name of configurator
QString GlobalConfigWidget::configName ()
{
    return tr ("Global");
}

// title string for this configurator
QString GlobalConfigWidget::configTitle ()
{
    return tr ("Global settings");
}

// return icon of this configurator
QPixmap GlobalConfigWidget::icon ()
{
    return QPixmap (":/src/icons/32/pikdev_config_general.png");
}

// apply configured data (i.e.: save them to config file)
void GlobalConfigWidget::doApply ()
{
    QString longprogname = programmers->currentText ();

    QVector<ProgrammerBase *>& progs = main_widget->programmersList ();

    // update paths for each tool
    for (int i = tool_names.size () - 1; i >= 0; --i)
      {
        QString path, URL, info, toolname = tool_names[i]->text ();
        bool mand;
        Config::getToolEntry (toolname, path, info, URL, mand);
        if (mand && tool_paths[i]->text () == "")
	  {

            QMessageBox::warning (this, tr ("Installation needed"),
				  tr ("Pikdev needs %1 to work properly.\nPlease install this tool (see %2)").arg (toolname).arg (URL));
	  }
        else
	  {
            QString exename = tool_paths[i]->text ();
            if (QFile::exists (exename))
	      Config::setToolPath (toolname, exename);
            else if (exename != "")
	      {
		QMessageBox::warning (this, tr ("Incorrect path"),
				      tr ("External tool %1 doesn't exist.\nPlease fix this path.").arg (exename));
	      }
	  }
      }

    // get programmer long name from combobox
    // and find corresponding short name
    QString programmername;
    for (int i = progs.size () - 1; i >= 0; --i)
      {
        if (longprogname == progs[i]->longName ())
	  {
            programmername = progs[i]->shortName ();
            break;
	  }
      }

    // is programmer installed & configured ?
    QString path;
    Config::getToolPath (programmername, path);
    if (programmername != "Builtin" && path == "")
      {
        QMessageBox::warning (this, tr ("Installation needed"),
			      tr ("Programming tool not found.\nPlease install the application %1").arg (programmername));
        Config::setCurrentProgrammer ("Builtin");
      }
    else
      Config::setCurrentProgrammer (programmername);

    main_widget->updateProgrammer ();

}

void GlobalConfigWidget::chooseExecutable (int i)
{
    QString fname = QFileDialog::getOpenFileName (this, "/usr", tr ("Executable (*)"));
    tool_paths[i]->setText (fname);
}
