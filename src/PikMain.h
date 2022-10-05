/***************************************************************************
    begin                : 2013
    copyright            : 2013 by Alain Gibaud
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

#ifndef PIKMAIN_H
#define PIKMAIN_H

#include <QMenuBar>
#include <QStatusBar>
#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QToolBar>
#include <QVector>

#include "codeeditor/QBEditor.h"
#include "ConfigCenter.h"
#include "DocBrowser.h"
#include "PicFactory.hh"
#include "PrgWid.h"
#include "ProjectWid.h"


class ConfigCenter;
class ProjectWid;
class PikMain : public QMainWindow
{
    Q_OBJECT

public:
    static PikMain *mainWindow () { return self; }
    PikMain ();
    QAction *act (const QString& a);

    int loadConfigFile ();
    int saveConfigFile ();
    /* -----------------------------------------------------------------------
       Parse a source file to find which device has to be programmed.
       Device name is returned in device.

       Returns:
       1 == OK
       0 == file not found
       ----------------------------------------------------------------------- */
    int snifDeviceName (const QString& filename, QString& device);

    /* ----------------------------------------------------------------------- 
       Write a command to console, and execute it.
       Error messages are written back to the console with red ink
       
       Returns:
       status of the executed command
       ----------------------------------------------------------------------- */
    int executeCommand (const QString& com);

    QBEditor *editor ();
    // Accessor for the project widget
    ProjectWid *projectWid ();
    // Accessor for current programmer
    ProgrammerBase *programmer ();
    // Performs clean up BEFORE the window is closed.
    virtual bool queryClose ();
    
    QVector<ProgrammerBase *>& programmersList ();
    
    QString updateProgrammer ();
    // Build a list of processors from gpasm output
    QStringList getProcList ();
    // Build a list of processors from gpdasm output
    QStringList getDProcList ();
    QTextEdit *getConsole ();
    void createToolCollection ();
    void makeMenus ();
    void makeToolBars ();
    void getConsoleCursorPosition (int &l, int& c);

public slots:
    void notImplemented ();
    void assemblePrg ();
    void assembleFile ();
    void assembleProject ();
    void compileCProject ();
    void testWidget ();
    void burnChip ();
    void burnChipProject ();
    void burnChipFile ();
    void displayMessage (QString&);
    void consoleClicked ();
    void programmerClosed ();
    void exitApplication ();
    // Test stub
    void testCode ();    
    void slotOpenRecentProject (const QString& fullName);    
    void disAssemblePrg ();    
    void slotShowHex (const QString& fname);
    // Try to run the "pikloops" code generator for delays
    void slotRunPikloops ();    
    void slotConfigCenter ();    
    void slotConfigCenterClosed ();
    void slotAboutPikdev ();
    // invoke the documentation browser
    void browseDocumentation ();
    // store last page viewed and close browser
    void slotBrowserClosed (const QString& page);

signals:
    void SigNewFile ();
    void SigOpenFile ();
    void SigSaveFile ();
    void SigSaveAsFile ();
    void SigCloseFile ();
    void SigQuitPrg ();
    void SigCutSelection ();
    void SigCopySelection ();
    void SigPasteSelection ();
    void SigAssemble ();
    void SigProgram ();
    void SigAbout ();

private:
    // unique toolbar
    QToolBar *toolbar;
    QBEditor *editor_;
    // EditorManager *edimanager;
    QTextEdit *console;
    // programmer widget
    PrgWid *programmer_widget;
    // project widget
    QString programmerFile;
    // project widget
    ProjectWid *project_wid;    
    ConfigCenter *config_center;
    // programmers vector
    QVector<ProgrammerBase *> the_programmers;
    // current programmer pointer (0 = none)
    ProgrammerBase *current_programmer;
    static PikMain *self;
    ActionsCollection actions;
    // documetation browser : 0 if not open
    DocBrowser *docBrowser;
    // last page viewed before closing the previous browser
    QString lastDocPage;
};

#endif
