#ifndef PRGWID_H
#define PRGWID_H

#include <QComboBox>
#include <QMainWindow>
#include <QProgressBar>

#include "ActionsCollection.h"
#include "Indicator.h"
#include "ProgrammerBase.h"
#include "hardware.hh"
#include "pic.hh"


class PrgWid : public QMainWindow, public progressListener
{
    Q_OBJECT
    QString fileName;
    pic *curpic;
    ProgrammerBase *programmer;
    ActionsCollection actions;

    // programmer activity indicator
    Indicator *progRunning;
    QLabel *statusLabel;

    // status Bar
    QStatusBar *statBar;

    // progress bar
    QProgressBar *progress;

    QLineEdit *fileNameLabel;
    QLineEdit *detected, *revdetected;

public:
    PrgWid (ProgrammerBase *pr, const QString& fn, QWidget *parent);
    virtual ~PrgWid ();
    void go ();
    /* ----------------------------------------------------------------------- 
       Verify the currently plugged device type
       
       Returns:
       true if detected device type is compatible with current pic model
       or if device type is not autodetectable
       ----------------------------------------------------------------------- */
    bool verifyDeviceType (const QString& type);
    bool verifyDeviceType ();
    // Called before the widget is closed
    virtual bool queryClose ();
    QStringList parseProgOptions (const QString& op);
    // specified by progressObserver interface
    virtual void updateProgressBar (int percent);
    virtual void updateProgressText (const char *);

private:
    void setStatus (bool active, const QString& txt);
    void setFileName (const QString& fname);

public slots:
    void quit ();
    void saveHex ();
    void saveHexAs ();
    void loadHex ();
    void loadHexAs ();

    void readPic ();
    void erasePic ();
    void verifyPic ();
    void programPic ();
    void slotDetectDeviceType ();
    void slotProgOptionSelected (int opt);
    void slotExtraMenuManagement ();
    void slotExecExtra (bool);

signals:
    void fileUpdated (const QString&);
    void bufferModified ();
    void messageGenerated (QString&);

protected:
    // Find a QAction "a" from the parent's actionCollection ()
    QAction *act (const char *a) const;
};

#endif
