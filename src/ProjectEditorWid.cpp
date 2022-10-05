/***************************************************************************
                      ProjectEditorWid.cpp  -  description
                             -------------------
    begin                : Mon Dec 8 2003
    copyright            : (C) 2003 by Alain Gibaud
    email                : alain.gibaud@univ-valenciennes.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cassert>
#include <iostream>
using namespace std;

#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QToolTip>

#include "ProjectEditorWid.h"
#include "AsmConfigWidget.h"


ProjectEditorWid::ProjectEditorWid (Project *proj, QWidget *parent,
				    __attribute__ ((unused)) const char *wname)
    : QMainWindow (parent), project (proj)
{
    setAttribute (Qt::WA_DeleteOnClose);
    QIcon iconset (":/src/icons/22/pikdev_openfile.png");

    main_widget = PikMain::mainWindow ();

    QStringList procs = main_widget->getProcList ();
    QStringList hfmt;
    (((hfmt += "inhx8m") += "inhx8s") += "inhx16") += "inhx32";
    QStringList rad;
    (rad += "dec") += "hex";
    QStringList warn;
    ((warn +="0") += "1") += "2";

    setWindowTitle (tr ("Project editor"));

    QWidget *central = new QWidget (this);
    setCentralWidget (central);
    QVBoxLayout *Lcentral = new QVBoxLayout (central);

    // Global data, top groupbox
    QGroupBox *globalG = new QGroupBox (tr ("Global data"), central);
    Lcentral->addWidget (globalG);
    QFormLayout *LglobalG = new QFormLayout (globalG);
    LglobalG->setMargin (20);
    LglobalG->setLabelAlignment (Qt::AlignLeft);

    // project name and version
    QString line1= tr ("<font color=#FF0000>%1</font>").arg (project->projectName ());
    QHBoxLayout *Lprname = new QHBoxLayout;
    QLabel *projName = new QLabel (line1);
    Lprname->addWidget (projName);
    Lprname->addSpacing (30);
    Lprname->addWidget (new QLabel (tr ("Type")));
    line1 = tr ("<font color=#FF0000>%1</font>").arg (project->getType ());
    Lprname->addWidget (new QLabel (line1));
    Lprname->addSpacing (30);
    Lprname->addStretch ();
    Lprname->addWidget (new QLabel (tr ("Version")));
    glob_version = new QLineEdit (central);
    glob_version->setText (project->getVersion ());
    glob_version->setMaximumWidth (60);
    Lprname->addWidget (glob_version);

    LglobalG->addRow (tr ("Project name"), Lprname);

    line1 = tr ("<font color=#FF0000>%1</font>").arg (proj->projectPath ());
    LglobalG->addRow (tr ("Project path"), new QLabel (line1));

    glob_description = new QTextEdit (central);
    glob_description->setMaximumHeight (70);
    glob_description->setText (project->getDescription ());
    LglobalG->addRow (tr ("Description"), glob_description);

    int k;

    // assembler groupbox
    QGroupBox *asmG = new QGroupBox ("Assembler", central);
    Lcentral->addWidget (asmG);
    QGridLayout *LasmG = new QGridLayout (asmG);
    LasmG->setHorizontalSpacing (30); LasmG->setMargin (20);
    // Left and right form layouts
    QFormLayout *LasmGleft = new QFormLayout;
    LasmGleft->setLabelAlignment (Qt::AlignLeft);
    QFormLayout *LasmGright = new QFormLayout;
    LasmGright->setLabelAlignment (Qt::AlignLeft);
    LasmG->addLayout (LasmGleft, 0, 0);
    LasmG->addLayout (LasmGright, 0, 1);

    // left side device
    asm_device = new QComboBox (central);
    asm_device->addItems (procs);

    QString p = project->getDevice ();
    if (((k = procs.indexOf (p)) == -1) && (k = procs.indexOf (p + " (P)")) == -1)
      k = 0;
    asm_device->setCurrentIndex (k);
    asm_device->setToolTip (tr ("PiKdev can only program processors marked with (P).\n"
				"For the development of new programming drivers, please see\n"
				"http://pikdev.free.fr, or contact the author."));
    LasmGleft->addRow (tr ("Processor"), asm_device);

    // radix
    asm_radix = new QComboBox (central);
    asm_radix->setToolTip (tr ("Choose the default radix used by gpasm to parse integer constants\n"
			       "* means that radix is specified by LIST directive."));
    asm_radix->addItem ("*");
    asm_radix->addItems (rad);
    QString r = project->getRadix ();
    k = rad.indexOf (r);
    asm_radix->setCurrentIndex (k + 1);
    LasmGleft->addRow (tr ("Radix"), asm_radix);

    // warning level
    asm_warning = new QComboBox (central);
    asm_warning->addItem ("*");
    asm_warning->addItems (warn);
    QString w = project->getWarnLevel ();
    k = warn.indexOf (w);
    asm_warning->setCurrentIndex (k + 1);
    asm_warning->setToolTip (tr ("For a cleaner code, please choose '0'.\n"
				 "* means that warning level is specified by LIST directive."));
    LasmGleft->addRow (tr ("Warning"), asm_warning);

    // right side
    // list option
    asm_list = new QCheckBox (central);
    asm_list->setChecked (project->getAsmList ());
    LasmGright->addRow (tr ("List file"), asm_list);

    // include directory
    QHBoxLayout *Ldir = new QHBoxLayout;

    asm_include = new QLineEdit (central);
    Ldir->addWidget (asm_include);
    QString i = project->getIncludeDir ();
    asm_include->setText (i);
    asm_include->setToolTip (tr ("By using the pseudovariable $(PRJPATH), you can specify an include directory\n"
				 "relative to the project directory."));
    QPushButton *incdir_button = new QPushButton (iconset, "", central);
    Ldir->addWidget (incdir_button);
    connect (incdir_button, SIGNAL (clicked ()), this, SLOT (slotSelectIncDir ()));
    LasmGright->addRow (tr ("Include directory"), Ldir);

    // other options
    asm_other = new QLineEdit (central);
    QString o = project->getOtherOptions ();
    asm_other->setText (o);
    asm_other->setToolTip (tr ("This item allows you to manually specify a gpasm \"exotic\" option.\n"
			       "The $(PRJPATH) pseudo variable if supported in this item as it is "
			       "in the \"Include Directories\" one."));
    LasmGright->addRow (tr ("Other options"), asm_other);

    if (project->isCproject ())
      {
        // C groupbox
        QGroupBox *CG = new QGroupBox ("C compiler", central);

        Lcentral->addWidget (CG);

        QGridLayout *LCG = new QGridLayout (CG);
	LCG->setHorizontalSpacing (30);
	LCG->setMargin (20);
        // Left and right form layouts
        QFormLayout *LCGleft = new QFormLayout;
	LCGleft->setLabelAlignment (Qt::AlignLeft);
        QFormLayout *LCGright = new QFormLayout;
	LCGright->setLabelAlignment (Qt::AlignLeft);
	LCGright->setSpacing (10);
        LCG->addLayout (LCGleft, 0, 0);
        LCG->addLayout (LCGright, 0, 1);

        // left side compiler
        C_compiler = new QComboBox (central);
        C_compiler->setEditable (false);
        C_compiler->addItem ("cpik");

        // find correct entry and display it
        QString the_compiler (project->getCCompiler ());
        int k;
        for (k = 0; k < C_compiler->count (); ++k)
	  if (C_compiler->itemText (k) == the_compiler)
	    break;
        if (k == C_compiler->count ())
	  k = 0; // not found, take first entry
        C_compiler->setCurrentIndex (k);
        C_compiler->setToolTip (tr ("Sorry, PiKdev currently supports only one compiler."));

        LCGleft->addRow (tr ("Compiler"), C_compiler);

        // Include Directories
        QHBoxLayout *Lcinc = new QHBoxLayout;
        C_include = new QLineEdit (central);
        C_include->setText (project->getCIncludeDir ());
        C_include->setToolTip (tr ("By using the pseudovariable $(PRJPATH), you can specify a directory\n"
				   "relative to the project directory. Comma-separated list of directories allowed."));
        QPushButton *Cincdir_button = new QPushButton (iconset, "", central);
        connect (Cincdir_button, SIGNAL (clicked ()), this, SLOT (slotSelectCIncDir ()));
        Lcinc->addWidget (C_include);
        Lcinc->addWidget (Cincdir_button);
        LCGleft->addRow (tr ("Include directory"), Lcinc);

        // Libs Directories
        QHBoxLayout *Lchdr = new QHBoxLayout;
        C_libs = new QLineEdit (central);
        C_libs->setText (project->getCLibsDir ());
        C_libs->setToolTip (tr ("By using the pseudovariable $(PRJPATH), you can specify a directory\n"
				"relative to the project directory. Comma-separated list of directories allowed."));
        QPushButton *Clibs_button = new QPushButton (iconset, "", central);
        connect (Clibs_button, SIGNAL (clicked ()), this, SLOT (slotSelectCLibDir ()));

        Lchdr->addWidget (C_libs);
        Lchdr->addWidget (Clibs_button);
        LCGleft->addRow (tr ("Libs directory"), Lchdr);

        // other options
        C_other = new QLineEdit (central);
        C_other->setText (project->getOtherCOptions ());
        C_other->setToolTip (tr ("This item allows you to manually specify a C \"exotic\" option.\n"
				 "The $(PRJPATH) pseudo variable if supported in this item as it is "
				 "in the \"Libraries Directory\" one."));

        LCGleft->addRow (tr ("Other options"), C_other);

        // right side
        C_jmpopt = new QCheckBox (central);
        C_jmpopt->setChecked (project->getCJmpOpt ());
        LCGright->addRow (tr ("Optimize jumps"), C_jmpopt);

        // display slb files option
        C_slb = new QCheckBox (central);
        C_slb->setChecked (project->getCSlb ());
        LCGright->addRow (tr ("Display slb files"), C_slb);

        // display asm files option
        C_asm = new QCheckBox (central);
        C_asm->setChecked (project->getCAsm ());
        LCGright->addRow (tr ("Display asm files"), C_asm);
      }
    else
      {
        // Linker groupbox
        QGroupBox *lnkG = new QGroupBox (tr ("Linker"), central);
        Lcentral->addWidget (lnkG);
        QGridLayout *LlnkG = new QGridLayout (lnkG);
	LlnkG->setHorizontalSpacing (30);
	LlnkG->setMargin (20);
        // Left and right form layouts
        QFormLayout *LlnkGleft = new QFormLayout;
	LlnkGleft->setLabelAlignment (Qt::AlignLeft);
        QFormLayout *LlnkGright = new QFormLayout;
	LlnkGright->setLabelAlignment (Qt::AlignLeft);
        LlnkG->addLayout (LlnkGleft, 0, 0);
        LlnkG->addLayout (LlnkGright, 0, 1);

        // Left side hex file
        link_fformat = new QComboBox (central);
        link_fformat->addItems (hfmt);
        QString h = project->getHexFormat ();
        if ((k = hfmt.indexOf (h)) == -1)
	  k = 0;
        link_fformat->setCurrentIndex (k);
        link_fformat->setToolTip (tr ("CAUTION: inhx8s format is currently not supported by the PiKdev programmer.\n"
				      "If you need this support, please contact the author."));
        LlnkGleft->addRow (tr ("Hex file format"), link_fformat);

        // debug option
        link_debug = new QCheckBox (central);
        link_debug->setChecked (project->getLinkDebug ());
        LlnkGleft->addRow (tr ("Debug messages"), link_debug);

        // map option
        link_map = new QCheckBox (central);
        link_map->setChecked (project->getLinkMap ());
        LlnkGleft->addRow (tr ("Map file"), link_map);

        // Right side
        // object/libs Directories
        link_include = new QLineEdit (central);
        link_include->setText (project->getLibsDir ());
        link_include->setToolTip (tr ("By using the pseudovariable $(PRJPATH), you can specify a directory\n"
				      "relative to the project directory:"));
        QPushButton *libdir_button = new QPushButton (iconset, "", central);
        QHBoxLayout *Llib = new QHBoxLayout;
        Llib->addWidget (link_include);
        Llib->addWidget (libdir_button);
        connect (libdir_button, SIGNAL (clicked ()), this, SLOT (slotSelectLibDir ()));
        LlnkGright->addRow (tr ("Libraries directory"), Llib);

        // other options
        link_other = new QLineEdit (central);
        link_other->setText (project->getOtherLinkOptions ());
        link_other->setToolTip (tr ("This item allows you to manually specify a gplink \"exotic\" option.\n"
				    "The $(PRJPATH) pseudo variable if supported in this item as it is "
				    "in the \"Libraries Directory\" one."));

        LlnkGright->addRow (tr ("Other options"), link_other);
      }

    Lcentral->addStretch ();

    QHBoxLayout *Lbuttons = new QHBoxLayout;
    Lcentral->addLayout (Lbuttons);
    Lbuttons->setSpacing (20);
    Lbuttons->setMargin (10);

    Lbuttons->insertStretch (1);
    QPushButton *bok = new QPushButton (tr ("Ok"), central);
    Lbuttons->addWidget (bok);
    QPushButton *bapply = new QPushButton (tr ("Apply"), central);
    Lbuttons->addWidget (bapply);
    QPushButton *bcancel = new QPushButton (tr ("Cancel"), central);
    Lbuttons->addWidget (bcancel);
    // Connect buttons
    connect (bcancel, SIGNAL (clicked ()), this, SLOT (slotCancel ()));
    connect (bapply, SIGNAL (clicked ()), this, SLOT (slotApply ()));
    connect (bok, SIGNAL (clicked ()), this, SLOT (slotOk ()));

    setWindowIcon (QPixmap (":/src/icons/16/pikdev_icon.png"));
    setMinimumWidth (600);

    show ();
}

ProjectEditorWid::~ProjectEditorWid ()
{
}

void ProjectEditorWid::slotCancel ()
{
    close ();
}

void ProjectEditorWid::slotApply ()
{
    project->setDescription (glob_description->toPlainText ());
    project->setVersion (glob_version->text ());

    QString dev = asm_device->currentText ();
    // remove the (P) suffix if it exists
    if (dev.right (4) == " (P)")
      dev.remove (dev.length () - 4, 4);
    project->setDevice (dev);
    project->setWarnLevel (asm_warning->currentText ());
    project->setOtherOptions (asm_other->text ());
    project->setIncludeDir (asm_include->text ());
    project->setRadix (asm_radix->currentText ());
    project->setAsmList (asm_list->isChecked ());

    if (project->isCproject ())
      {
        project->setCLibsDir (C_libs->text ());
        project->setOtherCOptions (C_other->text ());
        project->setCIncludeDir (C_include->text ());
        project->setCCompiler (C_compiler->currentText ());
        project->setCSlb (C_slb->isChecked ());
        project->setCJmpOpt (C_jmpopt->isChecked ());
        project->setCAsm (C_asm ->isChecked ());
      }
    else
      {
        project->setHexFormat (link_fformat->currentText ());
        project->setLinkDebug (link_debug->isChecked ());
        project->setLinkMap (link_map->isChecked ());
        project->setLibsDir (link_include->text ());
        project->setOtherLinkOptions (link_other->text ());
      }

    project->commit ();
}

void ProjectEditorWid::slotOk ()
{
    slotApply ();
    slotCancel ();
}

void ProjectEditorWid::slotSelectIncDir ()
{
    QString selected;  // not used
    QString dir = QFileDialog::getOpenFileName (this, tr ("Select include directory"), ".", "*",
						&selected, QFileDialog::ShowDirsOnly);

    if (! dir.isEmpty ())
      asm_include->setText (dir);
}

void ProjectEditorWid::slotSelectLibDir ()
{
    QString selected; // not used
    QString dir = QFileDialog::getOpenFileName (this, tr ("Select libraries directory"), ".", "*",
						&selected, QFileDialog::ShowDirsOnly);

    if (! dir.isEmpty ())
      link_include->setText (dir);
}

void ProjectEditorWid::slotSelectCIncDir ()
{
    QString selected; // not used
    QString dir = QFileDialog::getOpenFileName (this, tr ("Select include directory"), ".", "*",
						&selected, QFileDialog::ShowDirsOnly);

    if (! dir.isEmpty ())
      {
        QString t = C_include->text ();
        if (t == "")
	  C_include->setText (dir);
        else
	  C_include->setText (t + "," + dir);
      }
}

void ProjectEditorWid::slotSelectCLibDir ()
{
    QString selected; // not used
    QString dir = QFileDialog::getOpenFileName (this, tr ("Select libraries directory"), ".", "*",
						&selected, QFileDialog::ShowDirsOnly);
    if (! dir.isEmpty ())
      {
        QString t = C_libs->text ();
        if (t == "")
	  C_libs->setText (dir);
        else
	  C_libs->setText (t + "," + dir);
      }
}
