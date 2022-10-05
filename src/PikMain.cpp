#include <cassert>
#include <cstdio>
#include <iostream>
using namespace std;

#include <QApplication>
#include <QDateTime>
#include <QLayout>
#include <QMessageBox>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>

#include "../config.h"
#include "ActionsCollection.h"
#include "BinValueEditor.h"
#include "BuiltinProgrammer.h"
#include "codeeditor/SHlFactory.h"
#include "Config.h"
#include "ConfigBitsWizard.h"
#include "ConfigCenter.h"
#include "DocBrowser.h"
#include "FNU.h"
#include "pic.hh"
#include "pk2Programmer.h"
#include "pk2cmdProgrammer.h"
#include "pkpProgrammer.h"
#include "PrgConfigWidget.h"
#include "PrgWid.h"
#include "RecentItemsAction.h"
#include "TestWindow.h"


PikMain *PikMain::self = 0;

PikMain::PikMain () : QMainWindow (0), programmer_widget (0),
		      project_wid (0), config_center (0),
		      current_programmer (0), docBrowser(0)
{
    self = this;
    setWindowTitle (tr ("PiKdev for Qt5"));

    // create programmers pool
    the_programmers.push_back (new pk2cmdProgrammer);
    the_programmers.push_back (new BuiltinProgrammer);
    the_programmers.push_back (new pkpProgrammer);
    the_programmers.push_back (new pk2Programmer);

    // create external tools informations
    createToolCollection ();

    actions.addAction ("about-pikdev", QString (), tr ("About PiKdev"), QKeySequence (), this);
    actions.connect ("about-pikdev", this, SLOT (slotAboutPikdev ()));
    actions.addAction ("about-qt", QString (), tr ("About Qt"), QKeySequence (), this);
    actions.connect ("about-qt", qApp, SLOT (aboutQt ()));

    actions.addAction ("quit", ":/src/icons/22/pikdev_exit.png", tr ("&Quit"),
		       QKeySequence (tr ("Ctrl+Q")), this);
    actions.connect ("quit", this, SLOT (exitApplication ()));

    // project actions
    actions.addAction ("new-project", ":/src/icons/22/pikdev_createproject.png", tr ("New..."),
		       QKeySequence (), this);
    actions.addAction ("open-project", ":/src/icons/22/pikdev_openproject.png", tr ("Open..."),
		       QKeySequence (), this);

    // this is a special action for dynamic menus
    RecentItemsAction *a = new RecentItemsAction (tr ("Open Recent..."), this);
    a->addIconRule (QRegExp (".*\\.pikprj"), QIcon (":/src/icons/16/pikprj.png"));
    actions.addAction ("open-recent-project", a);
    connect (a, SIGNAL (itemSelected (QString)), this, SLOT (slotOpenRecentProject (QString)));

    actions.addAction ("edit-project", ":/src/icons/22/pikdev_editproject",
		       tr ("Edit project..."), QKeySequence (), this);
    actions.addAction ("close-project", ":/src/icons/22/pikdev_closeproject",
		       tr ("Close project"), QKeySequence (), this);
    actions.addAction ("edit-cfbits-project", ":/src/icons/22/pikdev_edit_cfbits",
		       tr ("Edit config bits"), QKeySequence (), this);
    actions.addAction ("add-to-project", ":/src/icons/22/pikdev_addfile", tr ("Add file"),
		       QKeySequence (), this);
    actions.addAction ("addcurrent-to-project", ":/src/icons/22/pikdev_addcurrentfile",
		       tr ("Add current file"), QKeySequence (), this);

    // tools actions
    actions.addAction ("assemble", ":/src/icons/22/pikdev_compile", tr ("&Assemble/Compile"),
		       QKeySequence (Qt::Key_F5), this);
    actions.connect ("assemble", this, SLOT (assemblePrg ()));
    actions.addAction ("deassemble", ":/src/icons/22/pikdev_decompile", tr ("&Disassemble"),
		       QKeySequence (Qt::Key_F5+Qt::SHIFT), this);
    actions.connect ("deassemble", this, SLOT (disAssemblePrg ()));
    actions.addAction ("program", ":/src/icons/22/pikdev_chip", tr ("&Program"),
		       QKeySequence (Qt::Key_F6), this);
    actions.connect ("program", this, SLOT (burnChip ()));

    actions.addAction ("chip-doc", ":/src/icons/22/pic-docs", tr ("Devices documentation"),
		       QKeySequence (), this);
    actions.connect ("chip-doc", this, SLOT (browseDocumentation ()));

    actions.addAction ("pikloops", "", tr ("Pikloops"), QKeySequence (), this);
    actions.connect ("pikloops", this, SLOT (slotRunPikloops ()));

    // configure actions
    actions.addAction ("configure-center", "", tr ("PiKdev"),
		       QKeySequence (Qt::CTRL+Qt::SHIFT+Qt::Key_P), this);
    actions.connect ("configure-center", this, SLOT (slotConfigCenter ()));

    QSplitter *splitv = new QSplitter (this);
    splitv->setOrientation (Qt::Horizontal);
    splitv->setOpaqueResize (true);

    project_wid = new ProjectWid (this, splitv, "project widget");
    actions.connect ("new-project", project_wid, SLOT (slotNewProject ()));
    actions.connect ("open-project", project_wid, SLOT (slotOpenProject ()));
    actions.connect ("edit-project", project_wid, SLOT (slotEditProject ()));
    actions.connect ("close-project", project_wid, SLOT (slotCloseProject ()));
    actions.connect ("add-to-project", project_wid, SLOT (slotInsertFile ()));
    actions.connect ("addcurrent-to-project", project_wid, SLOT (slotInsertCurrentFile ()));
    actions.connect ("edit-cfbits-project", project_wid, SLOT (slotEditConfigBits ()));

    QSplitter *splith = new QSplitter (splitv);
    splith->setOrientation (Qt::Vertical);

    editor_ = new QBEditor (splith);

    console = new QTextEdit (splith);
    console->setMinimumHeight (50);
    console->setAcceptRichText (false);
    console->setReadOnly (true);

    connect (console, SIGNAL (selectionChanged ()), this, SLOT (consoleClicked ()));

    splith->setOpaqueResize (true);
    QList<int> sz;
    sz += 200;
    sz += 30;
    splith->setSizes (sz);

    QList<int> szv;
    szv += 80;
    szv += 350;
    splitv->setSizes (szv);

    setCentralWidget (splitv);

    // status bar
    statusBar()->setSizeGripEnabled (true);
    statusBar()->showMessage (tr ("Welcome to PiKdev for Qt5"), 5000);
    loadConfigFile ();

    // configure indenters
    HighlighterBase *CHL = SHlFactory::factory ()->getSHl ("*.c");
    if (CHL)
      {
        QString astylepath;
        Config::getToolPath ("astyle", astylepath);
        CHL->setIndenter (astylepath
			  + " --mode=c --style=ansi --pad-oper --indent=spaces=3 %1 2>&1");
      }

    // probe available ports before any allocation
    PrgConfigWidget::availablePorts ();

    // get current programmer
    QString msg = updateProgrammer ();

    if (msg != "")
      {
        QMessageBox::warning (this,
			      tr ("Programmer configuration error"),
			      msg, "OK");
      }

#if 0
    // once everything is properly set up, open  editors or project
    // as specified by the command line
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs ();
    for (int i = 0; i < args->count (); i++)
      {
        QString name = args->arg (i);

        if (FNU::isProjectFile (name) && project_wid->getProject () == 0)
	  project_wid->slotOpenProjectFile (name); // OK, open this project
        else if (FNU::isAsmSource (name) || FNU::isCSource (name)
		 || FNU::isHexFile (name) || FNU::isIncFile (name))
	  {
            KBEditor *e = edimanager->findOrCreate (QString (args->arg (i)));
            e->slotOpen (args->url (i));
            if (FNU::isHexFile (e->fileName ()))
	      e->slotSetReadWrite (false);
	  }
      }
#endif

    // now, try to reopen project/files
    // note: project or files specified with command line args
    // have precedence over previous session ones.
    QString project_name;
    QStringList file_names;
    Config::getOpened (project_name, file_names);

    if (project_wid->getProject () == 0 && project_name != "")
      project_wid->slotOpenProjectFile (project_name);

    QStringList::const_iterator p;
    for (p = file_names.begin (); p != file_names.end (); ++p)
      {
        QUrl u(*p);
        if (editor_->document (u))
	  continue; // already open
        QBEditorDocument *doc = editor_->loadReloadFile (u);
        // loading may fail ...
        if (doc)
	  {
	    if (FNU::isHexFile (doc->fileFullName ()))
	      doc->setReadOnly (true);
	  }
        else
	  {
            QString txt = tr ("Unable to load the file <br><br><b>%1</b> <br><br>"
			      " Please check path and filename.<br>").arg (*p);
            QMessageBox::warning (this, tr ("File not found"), txt, "Ok");
	  }
      }

    QString gpasmpath, gpdasmpath, gplinkpath;
    // test for gpasm and others

    Config::getToolPath ("gpasm", gpasmpath);
    Config::getToolPath ("gpdasm", gpdasmpath);
    Config::getToolPath ("gplink", gplinkpath);

    if (gpasmpath == "" || gpdasmpath == "" || gplinkpath== "")
      {
        QMessageBox::warning (this, tr ("Paths to gputils tools are not set"),
			      tr ("<b>Please use the configuration menu to set them<br><br>"),
			      "Ok");
      }

    // TODO look at how to do that with Qt
    // setAutoSaveSettings () ; // auto save-restore toolbars parameters and window size
    makeMenus ();
    makeToolBars ();

    // restore GUI state
    QSettings settings;
    settings.beginGroup ("session");
    restoreGeometry (settings.value ("geometry").toByteArray ());
    restoreState (settings.value ("windowState").toByteArray ());
    settings.endGroup ();

    // window icon
    setWindowIcon (QPixmap (":/src/icons/16/pikdev_icon.png"));
    // application icon
    // setIcon (QPixmap (":/src/icons/64/pikdev-for-Qt4.png"));
}

void PikMain::makeMenus ()
{
    // file
    QMenu *file = new QMenu (tr ("&File"), this);

    file->addAction (editor_->act ("new"));
    file->addSeparator ();
    file->addAction (editor_->act ("open"));

    QAction *a = editor_->act ("open-recent");
    RecentItemsAction *ra = dynamic_cast<RecentItemsAction *>(a);
    // configure decorations for each type of files
    ra->addIconRule (QRegExp (".*\\.(c|C|CPP|cpp|cc|CC)"), QIcon (":/src/icons/16/csrc.png"));
    ra->addIconRule (QRegExp (".*\\.(h|H)"), QIcon (":/src/icons/16/chdr.png"));
    ra->addIconRule (QRegExp (".*\\.(hex|HEX)"), QIcon (":/src/icons/16/hex.png"));
    ra->addIconRule (QRegExp (".*\\.(slb|SLB)"), QIcon (":/src/icons/16/slb.png"));
    ra->addIconRule (QRegExp (".*\\.(lst)"), QIcon (":/src/icons/16/plain.png"));
    ra->addIconRule (QRegExp (".*\\.(asm|ASM)"), QIcon (":/src/icons/16/asm.png"));
    file->addAction (a);

    file->addAction (editor_->act ("save"));
    file->addAction (editor_->act ("save-as"));
    file->addAction (editor_->act ("save-all"));
    file->addAction (editor_->act ("close"));
    file->addAction (editor_->act ("close-all"));
    file->addSeparator ();
    // file->addAction (editor_->act ("print"));
    file->addSeparator ();
    file->addAction (actions.act ("quit"));

    // edit
    QMenu *edit = new QMenu (tr ("&Edit"), this);
    edit->addAction (editor_->act ("undo"));
    edit->addAction (editor_->act ("redo"));
    edit->addAction (editor_->act ("cut"));
    edit->addAction (editor_->act ("copy"));
    edit->addAction (editor_->act ("paste"));
    edit->addSeparator ();
    edit->addAction (editor_->act ("search-replace"));
    edit->addAction (editor_->act ("goto"));
    edit->addSeparator ();
    edit->addAction (editor_->act ("comment"));
    edit->addAction (editor_->act ("uncomment"));
    edit->addSeparator ();
    edit->addAction (editor_->act ("indent"));

    // Display menu
    QMenu *display = new QMenu (tr ("Display"), this);
    display->addAction (editor_->act ("bookmarks"));
    display->addAction (editor_->act ("line-numbers"));
    display->addAction (editor_->act ("zoom-in"));
    display->addAction (editor_->act ("zoom-out"));

    // Bookmarks menu
    // TODO, rewrite this according to the "action" paradigm
    QMenu *bookmarks = new QMenu (tr ("Bookmarks"));
    connect (bookmarks, SIGNAL (aboutToShow ()), editor_, SLOT (slotShowBookmarksMenu ()));

    // Project menu
    QMenu *project = new QMenu (tr ("Project"));
    project->addAction (act ("new-project"));
    project->addAction (act ("open-project"));
    project->addAction (act ("open-recent-project"));
    project->addAction (act ("edit-project"));
    project->addAction (act ("close-project"));
    project->addAction (act ("edit-cfbits-project"));
    project->addAction (act ("add-to-project"));
    project->addAction (act ("addcurrent-to-project"));

    // Tools menu
    QMenu *tools = new QMenu (tr ("Tools"));
    tools->addAction (act ("assemble"));
    tools->addAction (act ("deassemble"));
    tools->addAction (act ("program"));
    tools->addAction (act ("chip-doc"));
    tools->addSeparator ();
    tools->addAction (act ("pikloops"));

    // Configure menu
    QMenu *config = new QMenu (tr ("Configuration"));
    config->addAction (act ("configure-center"));

    // About menu
    QMenu *about = new QMenu (tr ("About"));
    about->addAction (act ("about-pikdev"));
    about->addAction (act ("about-qt"));

    menuBar ()->addMenu (file);
    menuBar ()->addMenu (edit);
    menuBar ()->addMenu (display);
    menuBar ()->addMenu (bookmarks);
    menuBar ()->addMenu (project);
    menuBar ()->addMenu (tools);
    menuBar ()->addMenu (config);
    menuBar ()->addMenu (about);
}

void PikMain::makeToolBars ()
{
    QToolBar *file = new QToolBar (tr ("File"), this);
    file->setObjectName ("FileTB");

    file->addAction (editor_->act ("new"));
    file->addAction (editor_->act ("open"));
    //file->addAction (editor_->act("open-recent"));
    file->addAction (editor_->act ("save"));
    file->addAction (editor_->act ("save-as"));
    file->addAction (editor_->act ("close"));
    // file->addAction (editor_->act ("close-all"));
    file->addSeparator ();
    // file->addAction (editor_->act ("print"));
    file->addSeparator ();
    file->addAction (actions.act ("quit"));

    QToolBar *edit = new QToolBar (tr ("Edit"), this);
    edit->setObjectName ("EditTB");

    edit->addAction (editor_->act ("undo"));
    edit->addAction (editor_->act ("redo"));
    edit->addAction (editor_->act ("cut"));
    edit->addAction (editor_->act ("copy"));
    edit->addAction (editor_->act ("paste"));
    edit->addSeparator ();
    edit->addAction (editor_->act ("search-replace"));
    edit->addAction (editor_->act ("goto"));
    edit->addSeparator ();
    // TODO : find icons for these
    //edit->addAction (editor_->act ("comment"));
    //edit->addAction (editor_->act ("uncomment"));
    edit->addSeparator ();
    // edit->addAction (editor_->act ("indent"));

    QToolBar *project = new QToolBar (tr ("Project"), this);
    project->setObjectName ("ProjectTB");
    project->addAction (act ("new-project"));
    project->addAction (act ("open-project"));
    // project->addAction (act ("open-recent-project"));
    project->addAction (act ("edit-project"));
    project->addAction (act ("close-project"));
    project->addAction (act ("edit-cfbits-project"));
    project->addAction (act ("add-to-project"));
    project->addAction (act ("addcurrent-to-project"));


    QToolBar *tools = new QToolBar (tr ("Tools"), this);
    tools->setObjectName ("ToolsTB");

    tools->addAction (act ("assemble"));
    tools->addAction (act ("deassemble"));
    tools->addAction (act ("program"));
    tools->addSeparator ();
    tools->addAction (act ("chip-doc"));
    // tools->addAction (act ("pikloops"));

    addToolBar (file);
    addToolBar (edit);
    addToolBar (project);
    addToolBar (tools);
}

//
// returns:
// 1 == error during file open
// 0 == OK
int PikMain::saveConfigFile ()
{
    QSettings settings;

    RecentItemsAction *a = dynamic_cast<RecentItemsAction *>(act ("open-recent-project"));
    a->saveItems ("recent-projects", settings);

    a = dynamic_cast<RecentItemsAction *>(editor_->act ("open-recent"));
    a->saveItems ("recent-files", settings);

    QStringList opened_files = editor_->getAllFileNames ();

    QString opened_project = "";
    Project *p = project_wid->getProject ();
    if (p)
      opened_project = p->projectPathFileName ();
    Config::setOpened (opened_project, opened_files);
    settings.sync ();

    return 0;
}

//
// returns:
// 1 = error during file open
// 2 = malformed config file
int PikMain::loadConfigFile ()
{
    QSettings settings;

    // set standard prog cards
    Config::writeStdHardwares ();

    // set up the recent project files menu
    RecentItemsAction *a = dynamic_cast<RecentItemsAction *>(act ("open-recent-project"));
    assert (a != 0);
    a->loadItems ("recent-projects", settings);

    a = dynamic_cast<RecentItemsAction *>(editor_->act ("open-recent"));
    assert (a != 0);
    a->loadItems ("recent-files", settings);

    // TODO : save/restore interface state
    // set toobars
    // applyMainWindowSettings (conf);

    return 0;
}

void PikMain::notImplemented ()
{
    QMessageBox::warning (0, tr ("Sorry"), tr ("Not yet implemented"),
			  QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
}

void PikMain::testWidget ()
{
    TestWindow *t = new TestWindow (0);
    t->show ();
}

void PikMain::assemblePrg ()
{
    Project *p = project_wid->getProject ();

    if (p == 0)
      assembleFile ();
    else if (p->getType () == "asm")
      assembleProject ();
    else if (p->getType () == "C")
      compileCProject ();
}

void PikMain::assembleProject ()
{
    Project *proj = project_wid->getProject ();
    if (proj == 0)
      return; // security

    QStringList files = proj->getAbsFiles ();
    QStringList::iterator i;
    QString command = "", x = "";
    console->setText ("");

    QString expanded_include = proj->getIncludeDir ();
    expanded_include.replace (QRegExp ("\\$\\(PRJPATH\\)"), proj->projectPath ());
    expanded_include.replace (QRegExp ("//"), "/"); // strip double /
    expanded_include.replace (QRegExp ("/$"), ""); // strip trailing /com2

    int filecount;

    // assemble specified files if needed
    for (i = files.begin (), filecount = 0; i != files.end (); ++i)
      {
        // save dirty buffers
        QBEditorDocument *doc = editor_->document (QUrl (*i));
        if (doc && doc->isModified ())
	  {
            doc->saveFile ();
            editor_->setDocument (doc);
	  }

        // count files to be really linked
        if (FNU::isAsmSource (*i) || FNU::isLibFile (*i) || FNU::isObjFile (*i))
	  ++filecount;

        // Do not process non-source or up-to-date source files
        if (! FNU::isAsmSource (*i) || FNU::isUpToDate (*i))
	  continue;
        Config::getToolPath ("gpasm", command);
        command += " -c";
        // -I option
        if (expanded_include != "")
	  {
            command += " -I \"";
            command += expanded_include;
            command += '"';
	  }
        // -p option
        command += (" -p " + proj->getDevice ());
        // -r option
        x = proj->getRadix ();
        if (x != "*" && x != "")
	  command += (" -r " + x);
        // -w option
        x = proj->getWarnLevel ();
        if (x != "*" && x != "")
	  command += (" -w " + x);
        // misc option
        x = proj->getOtherOptions ();
        if (x != "")
	  command += (" " + x);
        // source file
        command += " \"";
        command += *i;
        command += '"';

        QString listfile = FNU::asmName2LstName (*i);
        QBEditorDocument *lst;
        if (executeCommand (command) != 0)
	  return;

        if (proj->getAsmList ()) // asm OK, list needed ?
	  {
            lst = editor_->loadReloadFile (QUrl (listfile));
            if (lst)
	      lst->setReadOnly (true);
	  }
        else if ((lst = editor_->document (QUrl (listfile))))
	  {
            editor_->setDocument (lst);
            editor_->slotCloseCurrentDocument ();
	  }
      }

    if (filecount == 0)
      {
	QMessageBox::warning (0, tr ("Sorry"), tr ("Nothing to link"),
			      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        return;
      }

    // now, link application
    QString hexname (proj->getHexName ());
    Config::getToolPath ("gplink", command);
    command += " -o \"";
    command += hexname ;
    command += '"';

    // -a option
    x = proj->getHexFormat ();
    if (x != "")
      command += (" -a " + x);
    // -m option
    if (proj->getLinkMap ())
      command += (" -m ");
    // -d option
    if (proj->getLinkDebug ())
      command += (" -d ");
    // -I option
    x = proj->getLibsDir ();
    if (x != "")
      command += (" -I \"" + x + '"');
    // -s option
    bool lnk_scr_provided = false;
    for (i = files.begin (); i != files.end (); ++i)
      {
        if (FNU::isLnkScrFile (*i))
	  {
            if (! lnk_scr_provided)
	      {
                command += (" -s \"" + *i + '"');
                lnk_scr_provided = true;
	      }
            else
	      {
                QMessageBox::warning (0, tr ("Sorry"),
				      tr ("More than one .lkr file provided. Link aborted."),
				      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
                return;
	      }
	  }
      }
    // misc options
    x = proj->getOtherLinkOptions ();
    if (x != "")
      command += (" " + x);

    // object files corresponding to src files
    for (i = files.begin (); i != files.end (); ++i)
      {
        if (FNU::isAsmSource (*i))
	  command += (" \"" + FNU::asmName2ObjName (*i) + '"');
      }
    // pre assembled objects
    for (i = files.begin (); i != files.end (); ++i)
      {
        if (FNU::isObjFile (*i))
	  command += (" \"" + *i + '"');
      }
    // libs must be at the end of the command
    for (i = files.begin (); i != files.end (); ++i)
      {
        if (FNU::isLibFile (*i))
	  command += (" \"" + *i + '"');
      }
    if (executeCommand (command) == 0)
      {
        // open the corresponding hex read-only editor
        QUrl hexurl (hexname);
        QBEditorDocument *hex = editor_->loadReloadFile (hexurl);
        if (hex)
	  hex->setReadOnly (true);

        // if map requested, also open the map file
        QBEditorDocument *map;
        QString mapfile = FNU::changeExt (proj->projectPathFileName (), "map");
        QUrl mapurl (mapfile);
        if (proj->getLinkMap ())
	  {
            map = editor_->loadReloadFile (mapurl);
            if (map)
	      map->setReadOnly (true);
	  }
        else if ((map = editor_->document (mapurl)))
	  {
            editor_->setDocument (map);
            editor_->slotCloseCurrentDocument ();
	  }
      }
}

void PikMain::assembleFile ()
{
    QString txt, includedirs, others, warnlevel, proc, hexformat, radix;

    int status;
    QBEditorDocument *e = editor_->currentDocument ();

    Config::getAsmConfig (proc, hexformat, includedirs, radix, warnlevel, others);

    if (e != 0)
      {
        if (! FNU::isAsmSource (e->fileFullName ()))
	  {
            QMessageBox::warning (0, "Sorry", tr ("Current buffer is not an asm source file"),
				  QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
            return;
	  }
        if (e->isModified ())
	  e->saveFile ();  // buffer is now systematically saved

        // expension of $(SRCPATH) in includedirs
        QString expanded_includedirs = includedirs;
        expanded_includedirs.replace (QRegExp ("\\$\\(SRCPATH\\)"), e->path ());
        // eventually remove
        expanded_includedirs.replace (QRegExp ("//"), "/");
        // and in others sub-command
        QString expanded_others = others;
        expanded_others.replace (QRegExp ("\\$\\(SRCPATH\\)"), e->path ());
        // eventually remove
        expanded_others.replace (QRegExp ("//"), "/");

        console->setText ("");
        QString hexname = FNU::asmName2HexName (e->fileFullName ());
        QString command;
        Config::getToolPath ("gpasm", command);
        command += " -L -o ";

        command += '"';

        command += hexname;
        command += '"';
        // parameter == * means that assembler must use hard-coded directive
        // provided by the source file.
        if (warnlevel != "*")
	  command += " -w ", command += warnlevel;
        if (expanded_includedirs != "")
	  command += " -I \"", command += expanded_includedirs, command += '"';
        if (proc != "*")
	  command += " -p ", command += proc;
        if (hexformat != "*")
	  command += " -a ", command += hexformat;
        if (radix != "*")
	  command += " -r ", command += radix;
        if (expanded_others != "")
	  command += " ", command += expanded_others;
        command += " \"";
        command += e->fileFullName ();
        command += '"';

        status = executeCommand (command);
        if (status == 0) // no problem
	  {
            // open the corresponding list read-only editor
            QString lstname = FNU::asmName2LstName (e->fileFullName ());
            if (lstname != "")
	      {
                QBEditorDocument *lst = editor_->loadReloadFile (QUrl (lstname));
                if (lst)
		  lst->setReadOnly(true);
	      }

            // open the corresponding hex read-only editor
            QString hexname = FNU::asmName2HexName (e->fileFullName ());
            if (hexname != "")
	      {
                QBEditorDocument *hex = editor_->loadReloadFile (QUrl (hexname));
                if (hex)
		  hex->setReadOnly (true);
	      }
	  }
      }
}

void PikMain::burnChip ()
{
    // some persons want to burn chips without
    // having correctly configured port

    if (current_programmer == 0)
      {
        QMessageBox::warning (0, tr ("Sorry"),
			      tr ("No programmer available."),
			      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        return;
      }

    if (! current_programmer->connectionOK ())
      {

        QMessageBox alert (QMessageBox::Critical, tr ("Sorry"),
			   tr ("Hardware port not available"),
                           QMessageBox::Ok, this);

        alert.setDetailedText (tr ("Parallel port (/dev/parport0, /dev/parports/0, etc.)\n"
				   "or\n"
				   "Serial port (/dev/ttyS0, /dev/ttyS1, etc.)\n"
				   "must exist and must be RW enabled.\n\n"
				   "See http://pikdev.free.fr for details."));
        alert.exec ();
        return;
      }

    if (project_wid->getProject ())
      burnChipProject ();
    else
      burnChipFile ();
}

// Calls the programmer widget for the currently open project
void PikMain::burnChipProject ()
{
    Project *proj = project_wid->getProject ();
    if (proj == 0)
      return;

    QString hexfile = proj->getHexName ();
    QString device = proj->getDevice ();

    if (! current_programmer->selectDevice (device))
      {
        QMessageBox::warning (0, tr ("Sorry"),
			      tr ("This pic device is not supported (%1).").arg (device),
			      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        return;
      }

    // try to open hex file
    const char *txt = current_programmer->device ()->loadHexFile (hexfile.toLatin1 ());
    if (txt != 0)
      {
        QString mess (tr ("Loading problem on <b>%1</b>: %2 "
			  "<br><br>Do you want to open the programming window anyway?")
		      .arg (hexfile).arg (txt));
        if (QMessageBox::warning (0, tr ("Sorry"), mess,
				  QMessageBox::Yes, QMessageBox::No, Qt::NoButton)
	    == QMessageBox::No)
	  return;
      }
    programmerFile = hexfile; // OK

    if (programmer_widget != 0)
      {
        // A programmer is still active : close it and reopen a new one
        programmer_widget->close ();
        programmer_widget = 0;
      }

    programmer_widget = new PrgWid (current_programmer, programmerFile, 0);
    // connect programmer's messages to console
    connect (programmer_widget, SIGNAL (messageGenerated (QString&)),
	     this, SLOT (displayMessage (QString&)));
    // proper signaling of programmer close
    connect (programmer_widget, SIGNAL (destroyed ()),
	     this, SLOT (programmerClosed ()));
    connect (programmer_widget, SIGNAL (fileUpdated (const QString&)),
	     this, SLOT (slotShowHex (const QString&)));

    programmer_widget->go ();
}

// Calls the programmer widget for the currently open hex file
void PikMain::burnChipFile ()
{
    QString proc, hexformat, includedirs, radix, warnlevel, others;

    Config::getAsmConfig (proc, hexformat, includedirs, radix, warnlevel, others);
    // if an hex file is open, get it as the default data
    // to be programmed
    QBEditorDocument *e = editor_->currentDocument ();

    if (e == 0)
      {
        QMessageBox::warning (0, tr ("Sorry"),
			      tr ("No buffer available."),
			      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        return;
      }
    QString filename;
    // find the target pic flavor
    // try with the name specifed in the asm configuration Widget
    QString device (proc);

    if (! current_programmer->selectDevice (device))
      {
        // the device name is perhaps not specified,
        // try to parse the source file to obtain it.
        filename = e->fileFullName ();
        if (FNU::isHexFile (filename))
	  {
            filename = FNU::hexName2AsmName (filename);
            snifDeviceName (filename, device);

            if (! current_programmer->selectDevice (device)) // try .ASM extension
	      {
                filename = FNU::changeExt (filename, "ASM");
                snifDeviceName (filename, device);  // last chance to get a device type ..
                current_programmer->selectDevice (device);
	      }
	  }
      }
    if (current_programmer->device () == 0)
      {
        if (device != "*")
	  {
            QMessageBox::warning (0, tr ("Sorry"),
				  tr ("This pic device  is not supported  (%1).")
				  .arg (device), QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
	  }
        else
	  {
            QMessageBox::warning (0, tr ("Sorry"),
				  tr ("Target device is not configured and cannot be guessed by source code analysis<br><br>"
				      "Possible causes:<br><br>-The current buffer is not an hex file,"
				      "<br>-The corresponding source file cannot be found,"
				      "<br>-This source file does not contain any processor directive."),
				  QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
	  }
        return;
      }

    // try to open the hex file
    if (e != 0 && FNU::isHexFile (e->fileFullName ()))
      {
        filename = e->fileFullName ();
        const char *txt = current_programmer->device ()->loadHexFile (filename.toLatin1 ());

        if (txt != 0)
	  {
            QMessageBox::warning (0, tr ("Sorry"), txt + filename,
				  QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
            return;
	  }
        programmerFile = filename; // OK
      }

    if (programmer_widget != 0)
      {
        // A programmer is still active : close it and open a new one
        programmer_widget->close ();
        programmer_widget = 0;
      }

    programmer_widget = new PrgWid (current_programmer, programmerFile, 0);
    // connect programmer messages to console
    connect (programmer_widget, SIGNAL (messageGenerated (QString&)),
	     this, SLOT (displayMessage (QString&)));
    // signaling programmer close
    connect (programmer_widget, SIGNAL (destroyed ()),
	     this, SLOT (programmerClosed ()));
    // signaling writing file
    connect (programmer_widget, SIGNAL (fileUpdated (const QString&)),
	     this, SLOT (slotShowHex (const QString&)));
    programmer_widget->go ();
}

void PikMain::displayMessage (QString& txt)
{
    // remove trailing newline
    // txt.truncate (txt.size () - 1);
    console->setText (txt);
    cout << txt.toStdString () << endl;
}

// Process user clicks in text console
void PikMain::consoleClicked ()
{
    int p1, i1;
    getConsoleCursorPosition (p1, i1);
    QTextDocument *doc = console->document ();
    QTextBlock block = doc->findBlockByLineNumber (p1 - 1);
    QString txt = block.text ();

    // static QRegExp r ("^([^:]*):([0-9]+):([^:]*)$");
    static QRegExp r ("^([^:]*):([0-9]+):.*$");
    r.indexIn (txt);

    QStringList list = r.capturedTexts ();

    if (list.count () == 3)
      {
        QString ln = list[2];
        if (! ln.isNull ())
	  {
            QBEditorDocument *e = editor_->switchOrLoad (list[1]);
            if (e != 0)
	      editor_->slotFlashLine (ln.toInt ());
	  }
      }
}

void PikMain::getConsoleCursorPosition(int &l, int& c)
{
    QTextCursor cursor = console->textCursor ();
    l = cursor.blockNumber () + 1;
    c = cursor.columnNumber () + 1;
}

void PikMain::programmerClosed ()
{
    programmer_widget = 0;
}

void PikMain::exitApplication ()
{
    saveConfigFile ();
    QSettings qs;
    editor_->saveSettings (qs);
    editor_->slotAboutToClose ();
    // editor_->slotCloseAllDocuments ();

    if (programmer_widget)
      programmer_widget->queryClose ();

    QSettings settings;
    settings.beginGroup ("session");
    settings.setValue ("geometry", saveGeometry ());
    settings.setValue ("windowState", saveState ());
    settings.endGroup ();
    settings.sync ();

    qApp->quit ();
}


/* ----------------------------------------------------------------------- 
   Parse a source file to find which device has to be programmed.

   Parameters:
   "device" The returned device name (may be empty)

   Returns:
   1 == OK
   0 == file not found
   ----------------------------------------------------------------------- */
int PikMain::snifDeviceName (const QString& sourcefilename, QString& device)
{
    QFile f (sourcefilename);
    int k;
    if (! f.open (QIODevice::ReadOnly))
      return 0;
    QTextStream ts (&f);

    device = "";
    QString line;
    // QRegExp re1 ("^[ \\t]+(?:PROCESSOR|processor)[ \\t]+((?:p|sx|P|SX)[a-z0-9A-Z]+)");
    QRegExp re1 ("^[ \\t]+(?:PROCESSOR|processor)[ \\t]+([a-z0-9A-Z]+)");
    QRegExp re2 ("^[ \\t]+(?:LIST|list)[ \\t]+");
    while (! ts.atEnd ())
      {
        line = ts.readLine (10000);
        // search PROCESSOR directive
        if (re1.indexIn (line, 0) != -1)
	  {
            device = re1.cap (1);
            break;
	  }
        // search LIST p=... directive
        if ((k = re2.indexIn (line, 0)) != -1)
	  {
            //QRegExp re3 ("(?:p|P)[ \\t]*=[ \\t]*((?:p|sx|P|SX)[a-z0-9A-Z]+)") ;
            QRegExp re3 ("(?:p|P)[ \\t]*=[ \\t]*([a-z0-9A-Z]+)");
            if (re3.indexIn (line, k + 5) != -1)
	      {
                device = re3.cap (1);
                break;
	      }
	  }
      }
    // performs some magics to make device name gpasm conformant.
    device = device.toLower ();
    if (device != "" && device[0] != 'p')
      device = "p" + device; // add p prefix if missing
    return 1;
}

// Test stub : activated from F11 key for development purpose only
void PikMain::testCode ()
{
}

/* -----------------------------------------------------------------------
   Write a command to console, and execute it.

   Error messages are written back to the console with red ink
   Return:
   status of the executed command
   ----------------------------------------------------------------------- */
int PikMain::executeCommand (const QString& command)
{
    FILE *f;
    int status = -1;
    static QRegExp asm_error ("^([^:]*):([0-9]+):([^:]*)$");
    static QRegExp link_error ("^error:.*$");
    static QRegExp link_debug ("^debug:.*$");
    static QRegExp link_info ("^message:.*$");
    static QRegExp C_error ("^.*:([0-9]+): error:.*$");
    static QRegExp C_warning ("^.*:([0-9]+): warning:.*$");
    static QRegExp CPP_error ("^.*:([0-9]+):([0-9]+):.*:.*$");
    QColor color;

    console->setTextColor (QColor (0, 0, 0));
    console->append (command);
    f = popen ((command + " 2>&1").toLatin1 (),"r");
    if (f != 0)
      {
        char t[512];
        while (fgets (t, 511, f) == t)
	  {
            if (asm_error.indexIn (t) != -1 || C_error.indexIn (t) != -1
		|| CPP_error.indexIn (t) != -1)
	      color.setRgb (255, 0, 0); // ASM or C error (RED)
            else if (C_warning.indexIn (t) != -1)
	      color.setRgb (0xFF, 0x60, 0x2F); // C warning (ORANGE RED)
            else if (link_error.indexIn (t) != -1)
	      color.setRgb (180, 0, 0); // GPLINK error (DARK RED)
            else if (link_debug.indexIn (t) != -1)
	      color.setRgb (0, 0, 255); // GPLINK debug (BLUE)
            else if (link_info.indexIn (t) != -1)
	      color.setRgb (0, 160, 0); // GPLINK information (DARK GREEN)
            else
	      color.setRgb (0, 0, 0); // other (BLACK)
            console->setTextColor (color);
            int sz = strlen (t);
	    if (sz)
	      t[sz-1] = 0;
            console->append (t);
            console->setTextColor (QColor (0, 0, 0));
	  }
        status = pclose (f);
        if (status != 0)
            console->append (tr ("Error code: %1").arg (status));
      }
    else
        console->append (tr ("Cannot execute [%1]").arg (command));
    return status;
}

// Accessor for the project widget
ProjectWid *PikMain::projectWid ()
{
    return project_wid;
}

void PikMain::slotOpenRecentProject (const QString& fullName)
{
    project_wid->slotOpenProjectFile (fullName);
}

void PikMain::disAssemblePrg ()
{
    QString proc, other_opt;
    bool short_format;

    int status;
    QBEditorDocument *e = editor_->currentDocument ();

    // KBEditor *e = edimanager->currentEditor ();

    Config::getDAsmConfig (proc, short_format, other_opt);

    // in project mode, we use the project target device
    if (project_wid->getProject ())
      proc = project_wid->getProject ()->getDevice ();

    if (e != 0)
      {
        if (! FNU::isHexFile (e->fileFullName ()))
	  {
            QMessageBox::warning (0, "Sorry",
				  tr ("Current buffer is not an hex file"),
				  QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
            return;
	  }

        console->setText ("");
        QString hexname = e->fileFullName ();
        // file name xxx.hex => xxx.hex.asm
        QString outname = hexname + ".asm";
        QString command;
        Config::getToolPath ("gpdasm", command);
        command += " -p ";
        command += proc;
        command += ' ';
        if (short_format)
	  command += "-s ";
        command += hexname;
        command += " >";
        command += outname;

        status = executeCommand (command);
        if (status == 0) // no problem
	  {
            // open the corresponding  editor
            if (outname != "")
                editor_->loadReloadFile (outname);
	  }
      }
}

void PikMain::slotShowHex (const QString& fname)
{
    QBEditorDocument * e = editor_->loadReloadFile (fname);

    if (e)
      {
        console->setText ("");
        console->setTextColor (QColor (0, 160, 0));
        console->append (tr ("File %1 have been reloaded from disk.").arg (fname));
        console->setTextColor (QColor (0, 0, 0));
      }
}

/* Try to run the pikloops
   code generator for delays */
void PikMain::slotRunPikloops ()
{
    QString fullpath;
    Config::getToolPath ("pikloops", fullpath);
    if (fullpath != "")
      {
        fullpath += " 2>&1 &";
        FILE *f = popen (fullpath.toLatin1 (), "r");
        pclose (f);
      }
    else
      {
        QMessageBox alert (QMessageBox::Critical, tr ("Sorry"),
			   tr ("Cannot run Pikloops"),
                           QMessageBox::Ok, this);

        alert.setDetailedText (tr ("The Pikloops utility is not installed:"
				   " please contact Javier Fernando Vargas (fernandovargas@cable.net.co)"
				   " to obtain it.\n"
				   " You also can download pikloops at http://pikdev.free.fr"));

        alert.exec ();
      }
}

// Perform clean up BEFORE the window is closed.
bool PikMain::queryClose ()
{
    exitApplication ();
    return true;
}

void PikMain::slotConfigCenter ()
{
    if (config_center == 0)
      {
        config_center = new ConfigCenter (this);
        connect (config_center, SIGNAL (destroyed ()), this, SLOT (slotConfigCenterClosed ()));
      }
}

void PikMain::slotConfigCenterClosed ()
{
    config_center = 0;
}

void PikMain::compileCProject ()
{
    Project *proj = project_wid->getProject ();
    if (proj == 0)
      return; // security

    QStringList files = proj->getAbsFiles ();
    QStringList::iterator i;
    QString command = "", x = "";
    console->setText ("");

    QString expanded_include = proj->getCIncludeDir ();
    expanded_include.replace (QRegExp ("\\$\\(PRJPATH\\)"), proj->projectPath ());
    expanded_include.replace (QRegExp ("//"), "/"); // strip double /
    expanded_include.replace (QRegExp ("/$"), "");  // strip trailing /

    QString expanded_libs = proj->getCLibsDir ();
    expanded_libs.replace (QRegExp ("\\$\\(PRJPATH\\)"), proj->projectPath ());
    expanded_libs.replace (QRegExp ("//"), "/"); // strip double /
    expanded_libs.replace (QRegExp ("/$"), "");  // strip trailing /

    QString compiler = proj->getCCompiler ();

    if (compiler != "cpik")
      {
	QMessageBox::warning (0, tr ("Sorry"),
			      tr ("PiKdev currently only supports 'cpik'\n"
				  "C compiler for pic 18 devices"),
			      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        return;
      }

    int filecount;
    // Compile specified files if needed
    for (i = files.begin (), filecount = 0; i != files.end (); ++i)
      {
        // save dirty buffers
        QBEditorDocument *e = editor_->document (QUrl (*i));
        if (e && e->isModified ())
	  {
            e->saveFile ();
            editor_->setDocument (e); // TODO : probably not needed
	  }

        // count files to be really Linked
        if (FNU::isAsmSource (*i) || FNU::isSrcLib (*i) || FNU::isCSource (*i))
	  ++filecount;

        // Do not process non-source
        if (! FNU::isCSource (*i))
	  continue;

        Config::getToolPath ("cpik", command);
        command += " -c";
        // -I option

        if (expanded_include != "")
	  {
            command += " -I \"";
            command += expanded_include;
            command += '"';
	  }

        // -p option
        command += (" -p " + proj->getDevice ());

        // misc option
        x = proj->getOtherCOptions ();
        if (x != "")
	  command += (" " + x);
        // source file
        command += " \"";
        command += *i;
        command += '"';

        QString slbfile = FNU::CName2SlbName (*i);
        QBEditorDocument *slb = 0;
        if (executeCommand (command) != 0)
	  return;
        if (proj->getCSlb ()) // .slb files needed ?
	  {
            slb = editor_->loadReloadFile (QUrl (slbfile));
            slb->setReadOnly (true);
	  }
        else
	  {
            slb = editor_->switchToDocument (QUrl (slbfile));
            editor_->slotCloseCurrentDocument ();
	  }
      }

    if (filecount == 0)
      {
	QMessageBox::warning (0, tr ("Sorry"), tr ("Nothing to link"),
			      QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
        return;
      }

    // now, link application
    QString asmname (proj->getAsmName ());
    Config::getToolPath ("cpik", command);
    command += " -o \"";
    command += asmname;
    command += '"';
    command += (" -p " + proj->getDevice ());

    // misc options
    x = proj->getOtherLinkOptions ();
    if (x != "")
      command += (" " + x);

    // object files correspnding to src files
    for (i = files.begin (); i != files.end (); ++i)
      {
	if (FNU::isCSource (*i))
	  command += (" \"" + FNU::CName2SlbName (*i) + '"');
      }
    // pre compiled asm or slb
    for (i = files.begin (); i != files.end (); ++i)
      {
        if (FNU::isSrcLib (*i) || FNU::isAsmSource (*i))
	  command += (" \"" + *i + '"');
      }

    if (expanded_libs != "")
      {
        command += " -L ";
        command += ('"' + expanded_libs + '"');
      }

    QBEditorDocument *finalsrc;
    // TODO to continue

    if (executeCommand (command) == 0)
      {
        if (proj->getCAsm ())
	  {
            // open the corresponding  editor
            finalsrc = editor_->loadReloadFile (QUrl (asmname));
            // note : is resulting file editable ??
            // probably it should not be
            if (finalsrc)
	      finalsrc->setReadOnly (true);
	  }
        else
	  editor_->slotCloseDocument (QUrl (asmname));
      }
    else
      return;

    if (! proj->getCJmpOpt ())
      {
        expanded_include = proj->getIncludeDir ();
        expanded_include.replace (QRegExp ("\\$\\(PRJPATH\\)"), proj->projectPath ());
        expanded_include.replace (QRegExp ("//"), "/"); // strip double /
        expanded_include.replace (QRegExp ("/$"), ""); // strip trailing /com2

        QString hexname (proj->getHexName ());

        Config::getToolPath ("gpasm", command);
        command += " -L -o ";
        command += " \"";
        command += hexname;
        command += '"';

        // -I option
        if (expanded_include != "")
	  {
            command += " -I \"";
            command += expanded_include;
            command += '"';
	  }
        // -w option
        x = proj->getWarnLevel ();
        if (x != "*" && x != "")
	  command += (" -w " + x);
        // misc option
        x = proj->getOtherOptions ();
        if (x != "")
	  command += (" " + x);
        // source file
        command += " \"";
        command += asmname;
        command += '"';

        if (executeCommand (command) == 0)
	  {
            // open the corresponding editor
            QBEditorDocument *hex = editor_->loadReloadFile (QUrl (hexname));
            if (hex)
	      hex->setReadOnly (true);
            QString listname = FNU::asmName2LstName (asmname);
            if (proj->getAsmList ())
	      {
                QBEditorDocument *list = editor_->loadReloadFile (QUrl (listname));
                if (list)
		  list->setReadOnly (true);
	      }
            else
	      editor_->slotCloseDocument (QUrl (listname));
	  }
      }
    else
      {
        // use cpik to invoke the assembler
        // i.e.: cpik -a -A /usr/bin/gpasm -o xx.hex xx.src
        QString hexname (proj->getHexName ());
        QString asmbin;
        Config::getToolPath ("gpasm", asmbin);
        Config::getToolPath ("cpik", command);

        command += " -a -d1 -A ";
        command += " \"";
        command += asmbin;
        command += '"';
        // device
        command += (" -p " + proj->getDevice ());

        // output
        command += " -o ";
        command += " \"";
        command += hexname;
        command += '"';
        // input
        command += " \"";
        command += asmname;
        command += '"';


        if (executeCommand (command) == 0)
	  {
            // open the corresponding  editor
            QBEditorDocument *hex = editor_->loadReloadFile (QUrl (hexname));
            if (hex)
	      hex->setReadOnly (true);
            QString listname = FNU::asmName2LstName (asmname);
            if (proj->getAsmList ())
	      {
                QBEditorDocument *list = editor_->loadReloadFile (QUrl (listname));
                if (list)
		  list->setReadOnly (true);
	      }
            else
	      editor_->slotCloseDocument (QUrl (listname));
	  }
      }
}

QVector<ProgrammerBase *>& PikMain::programmersList ()
{
    return the_programmers;
}

// Remove current programmer and replace it by programmer specifed by config
QString PikMain::updateProgrammer ()
{
    QString progname;
    Config::getCurrentProgrammer (progname);
    int np;

    for (np = 0; np < the_programmers.size (); ++np)
      {
        if (the_programmers[np]->shortName () == progname)
	  {
            // before changing programmer, be sure
            // that programmer widget is closed
            if (programmer_widget)
	      {
                programmer_widget->close ();
                programmer_widget = 0;
	      }
            // if a programmer already installed, remove it
            if (current_programmer != 0)
	      current_programmer->exit ();
            current_programmer = the_programmers[np];

            return current_programmer->enter ();
	  }
      }
    return tr ("PikMain::updateProgrammer() - internal error, programmer [%1] not found")
      .arg (progname);
}

// ask gpasm for valid processors
QStringList PikMain::getProcList ()
{
    QStringList supported_procs = current_programmer->devices ();
    QString comm;
    Config::getToolPath ("gpasm", comm);
    comm += " -l 2>&1";

    FILE *f = popen (comm.toLatin1 (), "r");
    char name[60];
    QStringList list;

    if (current_programmer)
      {
        if (f != 0)
	  {
            while (fscanf (f, "%59s", name) != EOF)
	      {
                if (*name == 'p') // get pic names only
		  {
                    QString qname (name);
                    if (supported_procs.indexOf (qname) != -1)
		      qname += " (P)";
                    list += qname;
		  }
	      }
            pclose (f);
	  }
        else
	  list += "";
      }
    else
      list += "";
    return list;
}

QStringList PikMain::getDProcList ()
{
    QStringList list;
    QString comm;
    Config::getToolPath ("gpdasm", comm);
    if (comm != "")
      {
        comm += " -l 2>&1";
        FILE *f = popen (comm.toLatin1 (), "r");
        char name[60];

        if (f != 0)
	  {
            while (fscanf (f, "%59s", name) != EOF)
	      {
                if (*name == 'p')
		  list += name;
	      }
            pclose (f);
	  }
      }
    if (list.empty ())
      list += "";
    return list;
}

QTextEdit *PikMain::getConsole ()
{
    return console;
}

void PikMain::createToolCollection ()
{
    // empty tool list
    Config::removeToolList ();

    Config::createToolEntry ("gpasm",
			     tr ("gpasm is an assembler for PIC microcontrollers.\nIt is required by PiKdev to develop any application and is part of the gputils suite."),
			     "http://gputils.sourceforge.net/", true);

    Config::createToolEntry ("gpdasm",
			     tr ("gpasm is a disassembler for PIC microcontrollers.\nIt is recommended by PiKdev and is part of the gputils suite."),
			     "http://gputils.sourceforge.net/", true);

    Config::createToolEntry ("gplink",
			     tr ("gpasm is a linker for PIC microcontrollers.\nIt is required by PiKdev to develop any application and is part of the gputils suite."),
			     "http://gputils.sourceforge.net/", true);

    Config::createToolEntry ("pikloops",
			     tr ("pikloops is a tool for automatic generation of accurate delay routines.\nJust drag generated code to PiKdev editor."),
			     "http://pikdev.free.fr/", false);

    Config::createToolEntry ("astyle",
			     tr ("astyle performs automatic indentation of C source codes.\nYou only need it if you develop C programs with PiKdev."),
			     "http://sourceforge.net/projects/astyle", false);

    Config::createToolEntry ("cpik", tr ("cpik is a C compiler for pic18Fxxx devices.\ncpik is fully interfaced with PiKdev."),
			     "http://pikdev.free.fr", false);

    Config::createToolEntry ("pk2",
			     tr ("pk2 is a programming software for USB PICKit2 programmers from Microchip. This tool is now obsolete."),
			     "http://home.pacbell.net/theposts/picmicro/", false);

    Config::createToolEntry ("pk2cmd",
			     tr ("pk2cmd (version >= 1.20.0) is a programming software for USB PICKit2 programmers from Microchip. pk2cmd is a replacement for pk2."),
			     "http://www.microchip.com or http://pikdev.free.fr", false);

    Config::createToolEntry ("pkp",
			     tr ("pkp is a command line programmer which uses PiKdev programming engine.\nJust included here for development purpose."),
			     "http://pikdev.free.fr", false);
}

ProgrammerBase *PikMain::programmer ()
{
    return current_programmer;
}

QBEditor *PikMain::editor ()
{
    return editor_;
}

QAction *PikMain::act (const QString& a)
{
    QAction *qa = actions.act (a);
    // debug only, for undefined actions tracking
    if (0 == qa)
        cout << "FIXME: QAction *PikMain::act (const QString& a) : action "
                << a.toStdString() << " not found." << endl;
    return qa;
}

void PikMain::slotAboutPikdev ()
{
    QString msg = tr ("<b>PiKdev %1</b><br><br>Compiled on %2<br>"
		      "(c) 2002-2013 by Alain Gibaud (http://pikdev.free.fr)<br><br>"
		      "Credits:<br>"
		      "<i>Brian C. Lane</i>: His code for programming PICs helped me during my first developments.<br>"
		      "<i>Craig Franklin, Scott Dattalo, Marko Kohtala, Borut Razem and many other guys</i>: Nice gputils project.<br>"
		      "<i>Moln\xE0r K\xE0roly</i>: Documentation in HTML for PIC devices.<br>"
		      "<i>Manwlis \"Manos\" Giannos</i>: First Pic18F programming code.<br>"
		      "<i>Gerardo</i>: Parsing of the command line arguments.<br>"
		      "<i>Sean A. Walberg</i>: 16F676/630 support.<br>"
		      "<i>Mirko Panciri</i>: Support for programmers with bidirectionnal buffers.<br>"
		      "<i>Keith Baker</i>: Support for 16F73/74/76/77.<br>"
		      "<i>Javier Fernando Vargas G</i>: Help for program internationalization, Spanish translation.<br> "
		      "<i>Andre Lohan</i>: German translation.<br>"
		      "<i>Jochen Graeber</i>: Device donation.<br>"
		      "<i>Holger Olhm</i>: Device donation.<br>"
		      "<i>Brian Sipos</i>: Linker file support in project mode.<br> "
		      "<i>Jean-Claude Vandenhekke</i>: Support for 18F6525/8525/6621/8621.<br> "
		      "<i>Jeff Post</i>: Collaboration for pk2 support.<br>"
		      "<i>Walter Kicinski</i>: Collaboration for pk2cmd support - Thanks !<br><br>"
		      "<i>Poly Electronics</i>: Migration from Qt 4 to Qt 5<br> "
		      "<i>The Trolls</i>: <b>THE Qt library</b> (http://qt-project.org).<br>");

    QMessageBox mb (this) ;
    mb.setText (msg.arg (VERSION).arg (__DATE__));
    mb.setIconPixmap (QPixmap (":/src/icons/64/pikdev-for-Qt5.png"));
    QPushButton *b = mb.addButton ("OK", QMessageBox::AcceptRole);
    b->setIcon (QIcon (":/src/icons/16/check.png"));

    mb.exec ();
}

void PikMain::browseDocumentation ()
{
    if (docBrowser == 0)
      {
        docBrowser = new DocBrowser (lastDocPage);
        connect (docBrowser, SIGNAL (browserClosed (QString)), this,
		 SLOT (slotBrowserClosed (QString)));
      }
}

void PikMain::slotBrowserClosed (const QString& page)
{
    lastDocPage = page;
    delete docBrowser;
    docBrowser = 0;
}
