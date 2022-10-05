/* A widget for generation of config bits code source (C and ASM)
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef CONFIGBITSWIZARD_H
#define CONFIGBITSWIZARD_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QPoint>
#include <QTreeWidget>

#include "ConfigBits.h"
#include "Project.h"
#include "ProjectWid.h"


class ProjectWid;
class ConfigBitsWizard : public QMainWindow
{
    Q_OBJECT
  
public:
    ConfigBitsWizard (ProjectWid *parent);
    ~ConfigBitsWizard ();
    virtual void closeEvent (QCloseEvent *event);

private:
    deviceRecord *deviceData;
    Project *current_project;
    QTreeWidget *confView;
    ConfigBits *config;

protected:
    virtual bool queryClose ();

private slots:
    void slotContextMenu (const QPoint& globalposition);
    void slotWriteConfig ();
    void slotReadConfig ();
    void slotMakeCode ();
			
public slots:
    void slotClose ();
  
signals:
    void closed ();
};

#endif
