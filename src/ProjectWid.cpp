/***************************************************************************
                          ProjectWid.cpp  -  description
                             -------------------
    begin                : Sat Jun 5 2003
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

#include <QApplication>
#include <QColor>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPixmap>
#include <QPushButton>
#include <QStringList>
#include <QToolTip>

#include "codeanalyzer.h"
#include "ConfigBitsWizard.h"
#include "csourcefile.xpm.h"
#include "entrypointA_slb.xpm.h" // icons for asm entry points
#include "entrypointDataDecl.xpm.h" // icons for C entry points
#include "entrypointDataDef.xpm.h"
#include "entrypointFctDecl.xpm.h"
#include "entrypointFctDef.xpm.h"
#include "entrypointTypename.xpm.h"
#include "FNU.h"
#include "includefile.xpm.h"
#include "NewProject.h"
#include "objectfile.xpm.h"
#include "parser.h"
#include "PikMain.h"
#include "ProjectEditorWid.h"
#include "project.xpm.h"
#include "RecentItemsAction.h"
#include "sourcefile.xpm.h"
#include "XQListviewitem.h"


ProjectWid::ProjectWid (PikMain *mainwin, QWidget *parent,
			__attribute__ ((unused)) const char *name)
    : QFrame (parent), newproject (0), currentItem (0), project (0),
      main_window (mainwin), projecteditor (0), configbits_editor (0),
      entryPointTimer (0)
{
    setAttribute (Qt::WA_DeleteOnClose);
    QVBoxLayout *Lthis = new QVBoxLayout (this);
    projView = new QTreeWidget (this);
    Lthis->addWidget (projView);

    projView->setColumnCount (2);
    projView->setColumnWidth (0, 170);
    projView->setColumnWidth (1, 170);
    QStringList h;
    h << "Files" << "Info";
    projView->setHeaderLabels (h);

    h.clear ();
    h << tr ("  <Empty>  ") << "" << "p";

    projectTree = new QTreeWidgetItem (projView, h);
    projectTree->setIcon (0, QIcon (QPixmap ((const char **) project_xpm)));

    setContextMenuPolicy (Qt::CustomContextMenu);
    connect (this, SIGNAL (customContextMenuRequested (const QPoint&)),
             this, SLOT (slotContextMenu (const QPoint&)));

    connect (projView, SIGNAL (itemClicked (QTreeWidgetItem *, int)),
             this, SLOT (slotProjectElementClicked (QTreeWidgetItem *, int)));
    entryPointTimer = new QTimer (this);
    entryPointTimer->setSingleShot (false);
    connect (entryPointTimer, SIGNAL (timeout ()), this, SLOT (slotUpdateEntryPoints ()));
    entryPointTimer->start (5000); // start and reschedule timert

    slotProjectIsOpen (false);
    setAcceptDrops (true);
}

ProjectWid::~ProjectWid ()
{
}

void ProjectWid::slotContextMenu (const QPoint& pglob)
{
    QIcon iconopenproject (":/src/icons/22/pikdev_openproject.png");
    QIcon iconnewproject (":/src/icons/22/pikdev_createproject.png");
    QIcon iconeditproject (":/src/icons/22/pikdev_editproject.png");
    QIcon iconcloseproject (":/src/icons/22/pikdev_closeproject.png");
    QIcon iconaddfile (":/src/icons/22/pikdev_addfile.png");
    QIcon iconaddcurrentfile (":/src/icons/22/pikdev_addcurrentfile.png");
    QIcon iconbiteditor (":/src/icons/22/pikdev_edit_cfbits.png");

    // transform p to local widget coordinates
    QPoint p (pglob.x () + x (), pglob.y () + y ());
    QString menu_title;

    QList<QTreeWidgetItem *> items = projView->selectedItems ();
    if (items.size () == 1)
      {
        QTreeWidgetItem *li = items[0];
        if (li != 0)
	  {
            if (li->text (2) == "P")
	      {
                // "open" project entry
                QMenu *pop = makePopup (tr ("Project"), this);
                QAction *a1 = pop->addAction (iconaddfile, tr ("Insert file"));
                QAction *a2 = pop->addAction (iconaddcurrentfile, tr ("Insert current file"));
                QAction *a3 = pop->addAction (iconeditproject, tr ("Edit project"));
                QAction *a4 = pop->addAction (iconcloseproject, tr ("Close project"));
                QAction *a5 = 0;
                if (project) // project open ?
		  {
                    pop->addSeparator ();
                    a5 = pop->addAction (iconbiteditor, tr ("Edit device configuration bits"));
		  }

                QAction *a = pop->exec (QWidget::mapToGlobal (p));

                if (a == a1)
		  slotInsertFile ();
                else if (a == a2)
		  slotInsertCurrentFile ();
                else if (a == a3)
		  slotEditProject ();
                else if (a == a4)
		  slotCloseProject ();
                else if (a == a5)
		  slotEditConfigBits ();
	      }
            else if (li->text (2) == "p")
	      {
                // empty project entry
                QMenu *pop = makePopup ( tr ("Project"), this);
                QAction *a1 = pop->addAction (iconnewproject, tr ("Create project"));
                QAction *a2 = pop->addAction (iconopenproject, tr ("Open project"));

                QAction *a = pop->exec (QWidget::mapToGlobal (p));
                if (a == a1 && newproject == 0)
		  slotNewProject ();
                else if (a == a2 && newproject == 0)
		  slotOpenProject ();
	      }
            else
	      {
                QString code = li->text (2);
                if (code != "CE" && code != "RE")
		  {
                    if (code  == "A" || code == "C")
		      menu_title = tr ("Source file");
                    else if (code == "O")
		      menu_title = tr ("Object file");
                    else if (code == "L")
		      menu_title = tr ("Library file");
                    else if (code == "I")
		      menu_title = tr ("Include file");
                    else if (code == "S")
		      menu_title = tr ("Linker script file");
                    else if (code == "R")
		      menu_title = tr ("Source library file");
                    else if (code == "H")
		      menu_title = tr ("C header");
                    else
		      menu_title = tr ("Other file");
		    
                    QMenu *pop = makePopup (menu_title, this);

                    QAction *a1 = pop->addAction (QIcon (), tr ("Remove from project"));

                    QAction *a = pop->exec (QWidget::mapToGlobal (p));
                    if (a == a1)
		      slotRemoveFile (li);
		  }
	      }
	  }
      }
}

void ProjectWid::slotCreateProject (const QString& projectname, const QString& projectdir,
				    const QString& projecttype)
{
    QDir dir;
    // multiple project files in one directory are now allowed

    QString projfile (projectdir + "/" + projectname + ".pikprj");
    QFile f (projfile);
    // reject request if project file already exists
    if (f.exists ())
      {
        QMessageBox::critical (this, tr ("Sorry"),
			       tr ("The project file <b>%1</b> already exists").arg (projfile),
			       QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        return;
      }

    // create dir if it does not exist
    if (! dir.exists (projectdir) && ! dir.mkdir (projectdir))
      {
        QMessageBox::critical (this, tr ("Sorry"),
			       tr ("Cannot create directory <b>%1</b>").arg (projectdir),
			       QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        return;
      }

    if (project)
      delete project;

    project = new Project (projfile);
    // set minimal project data
    project->setDescription ("");
    project->setOutput (projectname + ".hex");
    project->setVersion ("0.1");
    project->setType (projecttype);
    project->commit ();

    projectTree->setText (2, "P"); // change status to "open project"
    if (projecttype == "asm")
      projectTree->setText (1, tr ("Asm project"));
    else if (projecttype == "C")
      projectTree->setText (1, tr ("C project"));

    projectTree->setText (0, projfile); // change label to file name
    slotProjectIsOpen (true);
}

void ProjectWid::slotNewProjectClosed ()
{
    newproject = 0;
}

void ProjectWid::slotCloseProject ()
{
    if (project)
      {
        // signals will reset pointers
        if (projecteditor)
	  projecteditor->close ();
        if (configbits_editor)
	  configbits_editor->close ();

        // clear ALL the tree widget
        projView->clear ();
        // then re-create the top level item, with the status 'closed'
        QStringList h;
        h << tr ("  <Empty>  ") << "" << "p";
        projectTree = new QTreeWidgetItem (projView, h);
        projectTree->setIcon (0, QIcon (QPixmap ((const char **) project_xpm)));

        //
        // close ALL open files !!
        main_window->editor ()->slotCloseAllDocuments ();

        slotProjectIsOpen (false);
        delete project;
        project = 0;
      }
}

// Display file selector and insert a new File to project
void ProjectWid::slotInsertFile ()
{
    if (project == 0)
      return; // security

    QString filename = QFileDialog::getOpenFileName (this, tr ("Select file"), ".",
						     tr ("Asm source (*.src *.slb *.asm *.SRC *.SLB *.ASM);;"
							 "C source (*.c *.C);;"
							 "C header (*.h *.H);;"
							 "Object file (*.o *.O);;"
							 "Library (*.slb *.lib *.a);;"
							 "Asm include file (*.inc *.INC);;"
							 "Linker script (*.lkr *.LKR)"), 0);

    if (filename != "")
      insertAbsFile (filename);
}

bool ProjectWid::insertAbsFile (const QString& fname)
{
    if (project->absFileExists (fname))
      return false; // already exists
    QString f = project->projectRelative (fname); // make filename project relative
    project->addFile (f);
    addFile2Tree (f);
    return true;
}

void ProjectWid::slotOpenProject ()
{
    if (newproject != 0)
      newproject->close ();
    if (project != 0)
      slotCloseProject (); // security

    QString projectfile = QFileDialog::getOpenFileName (this, tr ("Select project file"), ".",
							tr ("Pikdev project file (*.pikprj *.PIKPRJ"), 0);

    slotOpenProjectFile (projectfile);
}

/* Add a file name to the project tree
   filename may be:
   a) absolute
   b) relative to the project file location */
void ProjectWid::addFile2Tree (const QString& f)
{
    char **icon = (char **) sourcefile_xpm;
    QString type, code;

    if (FNU::isAsmSource (f))
      {
        icon = (char **) sourcefile_xpm;
        type = tr ("ASM file");
        code = "A";
      }
    if (FNU::isIncFile (f))
      {
        icon = (char **) includefile_xpm;
        type = tr ("Include file");
        code = "I";
      }
    else if (FNU::isCSource (f))
      {
        icon = (char **) csourcefile_xpm;
        type = tr ("C file");
        code = "C";
      }
    else if (FNU::isCHeader (f))
      {
        icon = (char **) includefile_xpm;
        type = tr ("C header");
        code = "H";
      }
    else if (FNU::isSrcLib (f))
      {
        icon = (char **) objectfile_xpm;
        type = tr ("Source Lib");
        code = "R";
      }
    else if (FNU::isObjFile (f))
      {
        icon = (char **) objectfile_xpm;
        type = tr ("Object file");
        code = "O";
      }
    else if (FNU::isLibFile (f))
      {
        icon = (char **) objectfile_xpm;
        type = tr ("Library file");
        code = "L";
      }
    else if (FNU::isLnkScrFile (f))
      {
        icon = (char **) sourcefile_xpm;
        type = tr ("Linker script file");
        code = "S";
      }

    QStringList L;
    L << f << type << code;

    QTreeWidgetItem *entry = new QTreeWidgetItem (projectTree, L);
    entry->setIcon (0, QIcon (QPixmap ((const char**) icon)));
    // open tree
    projectTree->setExpanded (true);
}

// Remove the file corresponding to the selected item
void ProjectWid::slotRemoveFile (QTreeWidgetItem *item)
{
    project->removeFile (item->text (0));
    delete item;
}

// Return the current project object (may be NULL)
Project *ProjectWid::getProject ()
{
    return project;
}

// Make an empty popup menu with "title" header
QMenu *ProjectWid::makePopup (const QString& txt, QWidget *parent)
{	
    QMenu *pop = new QMenu (txt, parent);
    // insert a fake entry as menu header
    pop->addAction (txt)->setEnabled (false);
    pop->addSeparator ();
    return pop;
}

QBEditor *ProjectWid::openEditor (QTreeWidgetItem *item)
{
    QBEditor *ed = main_window->editor ();

    QString file (project->absFilename (item->text (0)));

    if (ed && (ed->switchOrLoad (QUrl (file)) == 0))
      {
        QMessageBox::warning (0, tr ("Sorry"),
			      tr ("Cannot find file %1").arg (file),
			      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        ed = 0;
      }
    return ed;
}

void ProjectWid::slotProjectElementClicked (QTreeWidgetItem *item, int)
{
    QBEditor *ed = 0;
    // click on the widget background calls this slot with a NULL pointer !
    if (item == 0)
      return;

    QString type (item->text (2));
    // we only switch to source files
    if (type == "A" || type == "C" || type == "I"
	|| type == "H" || type == "R" || type == "S")
        ed = openEditor (item);
    else if (type == "RE" || type == "CE")
      {
        QTreeWidgetItem *parent = item->parent ();
        ed = openEditor (parent);
        // fetch line number
        int line = item->text (3).toInt ();
        // lines are internally numbered from zero, but displayed from 1
        if (ed)
	  ed->slotFlashLine (line + 1);
      }
    else
      QApplication::beep ();
}

void ProjectWid::slotInsertCurrentFile ()
{
    QBEditorDocument *doc = main_window->editor ()->currentDocument ();
    if (doc == 0)
      return;
    QString cur = doc->fileFullName ();

    if (FNU::isCSource (cur) || FNU::isAsmSource (cur)
	|| FNU::isIncFile (cur) || FNU::isCHeader (cur)
	|| FNU::isSrcLib (cur))
      insertAbsFile (cur);
    else
      QApplication::beep ();
}

// Invokes the Project Editor Widget
void ProjectWid::slotEditProject ()
{
    if (project && projecteditor == 0)
      {
        if (configbits_editor)
	  configbits_editor->close ();

        projecteditor = new ProjectEditorWid (project, 0, "project editor widget");
        connect (projecteditor, SIGNAL (destroyed ()), this, SLOT (slotProjectEditorClosed ()));
      }
}

void ProjectWid::slotNewProject ()
{
    if (project != 0 || newproject != 0)
      return;

    newproject = new NewProject;  // "new project" widget
    connect (newproject, SIGNAL (projectData (const QString&, const QString&, const QString&)),
	     this, SLOT (slotCreateProject (const QString&, const QString&, const QString&)));
    connect (newproject, SIGNAL (destroyed ()), this, SLOT (slotNewProjectClosed ()));
}

// Updates the UI when a project is opened/closed
void ProjectWid::slotProjectIsOpen (bool open)
{
    // set up action validity
    main_window->act ("new-project")->setEnabled (! open);
    main_window->act ("open-project")->setEnabled (! open);
    if (open)
      {
        RecentItemsAction *a = dynamic_cast< RecentItemsAction *>(main_window->act ("open-recent-project"));
        if (a)
	  a->addEntry (project->projectPathFileName ());
    }
    main_window->act ("open-recent-project")->setEnabled (! open);

    main_window->act ("close-project")->setEnabled (open);
    main_window->act ("edit-project")->setEnabled (open);
    main_window->act ("add-to-project")->setEnabled (open);
    main_window->act ("addcurrent-to-project")->setEnabled (open);
}

void ProjectWid::slotProjectEditorClosed ()
{
    projecteditor = 0;
}

void ProjectWid::slotOpenProjectFile (const QString& projectfile)
{
    if (projectfile == "")
      return;

    if (project)
      slotCloseProject ();

    project = new Project (projectfile);
    projectTree->setText (2, "P"); // change status to "open project"
    projectTree->setText (0, projectfile);

    if (project->getType () == "asm")
      projectTree->setText (1, tr ("Asm project"));
    else if (project->getType () == "C")
      projectTree->setText (1, tr ("C project"));
    else
      projectTree->setText (1, "");

    QStringList files = project->getFiles ();
    for (QStringList::iterator i = files.begin (); i != files.end (); ++i)
      addFile2Tree (*i);
    projectTree->setExpanded (true);
    slotProjectIsOpen (true);
}

// Drag and drop management
void ProjectWid::dragEnterEvent (QDragEnterEvent *ev)
{
    // accept if dragged object can be decoded as an URI
    if (ev->mimeData ()->hasFormat ("text/uri-list"))
      ev->acceptProposedAction ();
}
// Drop on project widget
void ProjectWid::dropEvent (QDropEvent *ev)
{
    QStringList files;
    if (ev->mimeData ()->hasUrls ())
      {
        QList<QUrl> lurls = ev->mimeData ()->urls ();
        for (int i = 0; i < lurls.count (); ++i)
	  files << FNU::noScheme (lurls[i]);

        // only one file name, and it is a project file and no project already open
        if (project == 0 && files.count () == 1 && FNU::isProjectFile (*(files.begin ())))
	  slotOpenProjectFile (*(files.begin ())); // yes, open the project
        else if (project != 0 && files.count () == 1 && FNU::isProjectFile (*(files.begin ())))
	  {
            // close current project, and open a new one
            slotCloseProject ();
            slotOpenProjectFile (*(files.begin ()));
	  }
        else if (project != 0)
	  {
            // try to include files in the current project
            for (QStringList::Iterator it = files.begin (); it != files.end (); ++it)
	      {
                if (FNU::isAsmSource (*it) || FNU::isCSource (*it) || FNU::isLibFile (*it)
		    || FNU::isObjFile (*it) || FNU::isIncFile (*it)
		    || FNU::isSrcLib (*it) || FNU::isCHeader (*it))
		  insertAbsFile (*it); // multiple insertions are checked by inserAbsFile ()
	      }
	  }
      }
}

/* This slot is typically activated from a timer
   but can be called anywhere else */
void ProjectWid::slotUpdateEntryPoints ()
{
    if (main_window->editor ())
      {
        for (int i = 0; i < projectTree->childCount (); ++i)
	  {
            QTreeWidgetItem *it = projectTree->child (i);
            // get file name
            QString absfilename = project->absFilename (it->text (0));

            if (FNU::isSrcLib (absfilename) || FNU::isAsmSource (absfilename))
	      {
                // get editor pointer
                QBEditorDocument *doc = main_window->editor ()->document (QUrl (absfilename));
                // update tree
                updateAsmEntryPoints (doc, it);
	      }
            else if (FNU::isCSource (absfilename) || FNU::isCHeader (absfilename))
	      {
                // get document pointer
                QBEditorDocument *doc = main_window->editor ()->document (QUrl (absfilename));
                // update tree
                updateCEntryPoints (doc, it);
	      }
	  }
      }
}

// remove all the entry points for this file
void ProjectWid::removeChildren (QTreeWidgetItem *it)
{
    for(; it->childCount ();)
      it->removeChild (it->child (0));
}

/* Parses the specified document and update the widget tree to
    display entry points found. (C code) */
void ProjectWid::updateCEntryPoints (QBEditorDocument *doc, QTreeWidgetItem *it)
{
    if (doc && ! doc->analyzed ())
      {
        // try to analyze
        Parser p;
        if (p.parse (doc) == 0)
	  {
            doc->setAnalyzed (true);

            // remove all entry points
            removeChildren (it);

            vector<SymbEntry>::iterator i;
            for (i = p.symbols ().begin (); i != p.symbols ().end (); ++i)
	      {
                int st = (*i).symbol_type;
                QString sline = QString::number ((*i).line);
                QString name = ((*i).name).c_str ();
                QString proto = ((*i).prototype).c_str ();
                const char **pix = 0;
                QString code;

                if (st & SymbEntry::Function)
		  {
                    pix = st & SymbEntry::Definition ? entrypointFctDef : entrypointFctDecl;
                    code = st & SymbEntry::Definition ? "F " : "f";
		  }
                else if (st & SymbEntry::Variable)
		  {
                    pix = st & SymbEntry::Definition ? entrypointDataDef : entrypointDataDecl;
                    code = st & SymbEntry::Definition ? "V" : "v";
		  }
                else if (st & SymbEntry::Typename)
		  {
                    pix = entrypointTypename;
                    code = "B";
		  }
                QStringList L;
                L << name << proto << "CE" << sline << code;
                XQListViewItem *entryItem = new XQListViewItem (it, L);

                entryItem->setIcon (0, QIcon (QPixmap (pix)));
	      }
	  }
        else
	  {
            // QApplication::beep ();
            // cout << "Parse error" << endl;
            // cout << p.getErrMessage () << endl;
	  }
      }
}


/* Parses the specified document and update the widget tree to
   display entry points found. (ASM code) */
void ProjectWid::updateAsmEntryPoints (QBEditorDocument *doc, QTreeWidgetItem *it)
{
    if (doc && ! doc->analyzed ())
      {
        // remove entry points items
        removeChildren (it);

        CodeAnalyzer ca;
        ca.analyze (doc);
        vector<EntryPointRecord>& ep = ca.entryPoints ();
        for (unsigned int i = 0; i < ep.size (); ++i)
	  {
            QString sline = QString::number (ep[i].line);
            QString name = ep[i].name;
            const char **pix = 0;
            QString prototype = ep[i].prototype;
            pix = entrypointA_slb;

            QStringList L;
	    L << name << prototype << "RE" << sline << "";
            XQListViewItem *entryItem = new XQListViewItem (it, L);
            entryItem->setIcon (0, QIcon (QPixmap (pix)));
	  }
        doc->setAnalyzed (true);
      }
}

void ProjectWid::slotEditConfigBits ()
{
    if (project == 0)
        return;

    QString device = project->getDevice ();
    if (pikDB::pikdb ()->getData (device.toLatin1 ()) == 0)
      {
        QMessageBox::warning (0, tr ("Sorry"),
			      tr ("%1 : unknown device.").arg (device),
			      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        return;
      }
    if (configbits_editor != 0)
      configbits_editor->close ();
    // since the project editor allows to change the device at any moment
    // I close it to preserve consistancy
    if (projecteditor != 0)
      projecteditor->close ();

    configbits_editor = new ConfigBitsWizard (0);
    connect (configbits_editor, SIGNAL (closed ()), this, SLOT (slotConfigBitsEditorClosed ()));
}

PikMain *ProjectWid::mainWindow ()
{
    return main_window;
}

void ProjectWid::slotConfigBitsEditorClosed ()
{
    configbits_editor = 0;
}
