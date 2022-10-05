 /***************************************************************************
                          ProjectWid.h  -  description
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
/*A widget to display and edit  informations about the current project
  @author Alain Gibaud */

#ifndef PROJECTWID_H
#define PROJECTWID_H

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFrame>
#include <QMenu>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "ConfigBitsWizard.h"
#include "NewProject.h"
#include "ProjectEditorWid.h"


class ConfigBitsWizard;
class PikMain;
class ProjectEditorWid;
class ProjectWid : public QFrame
{
    Q_OBJECT

public:
    ProjectWid (PikMain *mainwindow, QWidget *parent, const char *);
    ~ProjectWid ();
    /* Add a file name to the project tree
       (filename is absolute or relative to the project file location) */
    void addFile2Tree (const QString& name);
    // Returns the current project object (might be NULL)
    Project *getProject ();
    // return the main window
    PikMain *mainWindow ();
    // Drag and drop management
    virtual void dragEnterEvent (QDragEnterEvent *);
    // Drop on project widget
    virtual void dropEvent (QDropEvent *ev);

    void updateCEntryPoints (QBEditorDocument *doc, QTreeWidgetItem *it);
    void updateAsmEntryPoints (QBEditorDocument *doc, QTreeWidgetItem *it);

public slots:
    void slotContextMenu (const QPoint&);
    void slotCreateProject (const QString&, const QString&, const QString&);
    void slotNewProjectClosed ();
    void slotCloseProject ();
    void slotInsertFile ();
    void slotOpenProject ();
    // remove the current item from tree (and the file from project)
    void slotRemoveFile (QTreeWidgetItem *item);
    void slotInsertCurrentFile ();

    // Calls the Project Editor Widget
    void slotEditProject ();
  
    void slotNewProject ();
    // Updates the UI when a project is opened/closed
    void slotProjectIsOpen (bool open);

    void slotProjectEditorClosed ();
    void slotOpenProjectFile (const QString& filename);
    virtual void slotUpdateEntryPoints ();
  
    // activate config bits editor
    void slotEditConfigBits ();
    // do housekeeping when cf bits editor is closed
    void slotConfigBitsEditorClosed ();

private:
    // New project widget
    NewProject *newproject;

    // Last selected item
    QTreeWidgetItem *currentItem;

    // Project list view
    QTreeWidget *projView;

    // Root of Project tree
    QTreeWidgetItem *projectTree;
    Project *project;

    // Link to main window
    PikMain *main_window;

    // Project editor, non null when open
    ProjectEditorWid *projecteditor;

    // Config bits editor, non null when open
    ConfigBitsWizard *configbits_editor;

    // Timer for triggering of source code parsing
    QTimer *entryPointTimer;

private:
    // Make an empty popup menu with "title" header
    QMenu *makePopup (const QString& title, QWidget *parent);

    // insert a new absolute filename - returns true if OK, false if already exists
    bool insertAbsFile (const QString &fname);

    // remove all children of list
    void removeChildren (QTreeWidgetItem *list);

    /* open the document which corresponds to this item -
       returns address of the editor, or 0 if doc is not found */
    QBEditor *openEditor (QTreeWidgetItem *item);

private slots:
    void slotProjectElementClicked (QTreeWidgetItem *, int);

public:
};

#endif
