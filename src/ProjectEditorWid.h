/***************************************************************************
                       ProjectEditorWid.h  -  description
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
/* This widget allows to modify all project parameters, except project name and hex file name
   @author Alain Gibaud */

#ifndef PROJECTEDITORWID_H
#define PROJECTEDITORWID_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QWidget>

#include "PikMain.h"
#include "Project.h"


class PikMain;
class ProjectEditorWid : public QMainWindow
{
   Q_OBJECT

public:
    ProjectEditorWid (Project *proj, QWidget *parent, const char *name);
    ~ProjectEditorWid ();

private:
    Project *project;

    QTextEdit *glob_description;
    QLineEdit *glob_version;

    QComboBox *asm_device, *asm_warning, *asm_radix;
    QLineEdit *asm_other, *asm_include;
    QCheckBox *asm_list;

    QComboBox *link_fformat;
    QCheckBox *link_debug, *link_map;
    QLineEdit *link_include, *link_other;

    QComboBox *C_compiler;
    QLineEdit *C_libs, *C_include, *C_other;
    QCheckBox *C_slb, *C_asm, *C_jmpopt;

    PikMain *main_widget;
  
public slots:
    void slotOk ();
    void slotApply ();
    void slotCancel ();
    void slotSelectIncDir ();
    void slotSelectLibDir ();
    void slotSelectCIncDir ();
    void slotSelectCLibDir ();
};

#endif
