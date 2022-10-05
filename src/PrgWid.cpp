/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* Programmer widget */

#include <iostream>
#include <sstream>
using namespace std;

#include <QAction>
#include <QApplication>
#include <QApplication>
#include <QBoxLayout>
#include <QEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QScreen>
#include <QStatusBar>
#include <QStringList>
#include <QTimer>
#include <QToolBar>
#include <QToolTip>
#include <QVariant>

#include "BinWordsEditor.h"
#include "ConfigBits.h"
#include "MemoryDisplayer.h"
#include "PrgWid.h"


PrgWid::PrgWid (ProgrammerBase *programmer_, const QString& fn, QWidget *parent)
    : QMainWindow (parent), fileName (fn), curpic (programmer_->device ()),
      programmer (programmer_)
{

    FragBuffer *mem;
    int mem1, mem2, ee1, ee2, id1, id2, cf1, cf2, osccal_backup1, osccal_backup2;
    QString bits;
    unsigned int memdisplay_h = 10; // number of lines for programm memory displayer

    setAttribute (Qt::WA_DeleteOnClose);

    // for low resolution devices (i.e.: netbooks)
    QFont f = font ();
    if (QGuiApplication::primaryScreen ()->availableGeometry ().height () <= 600)
      f.setPointSize (6);
    else
      f.setPointSize (10);
    setFont (f);

    setWindowTitle (programmer->longName ());

    /* ----------------------------------------------------------------------- 
       Functions "file"
       ----------------------------------------------------------------------- */
    actions.addAction ("open-hex", ":/src/icons/22/document-open.png",
		       tr ("&Open"), QKeySequence (tr ("Ctrl+O")), this);
    actions.connect ("open-hex", this, SLOT (loadHexAs ()));

    actions.addAction ("reload-hex", ":/src/icons/22/document-revert.png",
		       tr ("Reloa&d"), QKeySequence (tr ("Ctrl+D")), this);
    actions.connect ("reload-hex", this, SLOT (loadHex ()));

    actions.addAction ("save-hex", ":/src/icons/22/document-save.png",
		       tr ("&Save"), QKeySequence (tr ("Ctrl+S")), this);
    actions.connect ("save-hex", this, SLOT (saveHex ()));

    actions.addAction ("saveas-hex", ":/src/icons/22/document-save-as.png",
		       tr ("&Save As"), QKeySequence (tr ("Ctrl+A")), this);
    actions.connect ("saveas-hex", this, SLOT (saveHexAs ()));

    actions.addAction ("quit-prog", ":/src/icons/22/pikdev_close.png",
		       tr ("&Quit Programmer"), QKeySequence (tr ("Ctrl+Q")), this);
    actions.connect ("quit-prog", this, SLOT (quit ()));

    /* ----------------------------------------------------------------------- 
       Functions "chip"
       ----------------------------------------------------------------------- */
    actions.addAction ("readchip", ":/src/icons/22/readchip.png", tr("&Read Pic"),
		       QKeySequence (tr ("Ctrl+R")), this);
    actions.connect ("readchip", this, SLOT (readPic ()));

    actions.addAction ("erasechip", ":/src/icons/22/erasechip.png", tr ("&Erase Pic"),
		       QKeySequence (tr ("Ctrl+E")), this);
    actions.connect ("erasechip", this, SLOT (erasePic ()));

    actions.addAction ("burnchip", ":/src/icons/22/pikdev_burnchip.png", tr ("&Program Pic"),
		       QKeySequence (Qt::Key_F), this);
    actions.connect ("burnchip", this, SLOT (programPic ()));

    actions.addAction ("verifychip", ":/src/icons/22/verifychip.png", tr ("&Verify Pic"),
		       QKeySequence (tr ("Ctrl+V")), this);
    actions.connect ("verifychip", this, SLOT (verifyPic ()));

    /* ----------------------------------------------------------------------- 
       Create menus and plug actions
       ----------------------------------------------------------------------- */
    QMenu *file = new QMenu ("&File",this);
    menuBar ()->addMenu (file) ;
    file->addAction (actions.act ("open-hex"));
    file->addAction (actions.act ("reload-hex"));
    file->addAction (actions.act ("save-hex"));
    file->addAction (actions.act ("saveas-hex"));
    file->addAction (actions.act ("quit-prog"));
    QToolBar *tfile = new QToolBar ("&File", this);
    addToolBar (tfile);
    tfile->addAction (actions.act ("open-hex"));
    tfile->addAction (actions.act ("reload-hex"));
    tfile->addAction (actions.act ("save-hex"));
    tfile->addAction (actions.act ("saveas-hex"));
    tfile->addAction (actions.act ("quit-prog"));

    QMenu *picops = new QMenu ("Pic Operations", this);
    menuBar ()->addMenu (picops);
    picops->addAction (actions.act ("readchip"));
    picops->addAction (actions.act ("erasechip"));
    picops->addAction (actions.act ("burnchip"));
    picops->addAction (actions.act ("verifychip"));

    QToolBar *tpicops = new QToolBar ("Pic Operations", this);
    addToolBar (tpicops);
    tpicops->addAction (actions.act ("readchip"));
    tpicops->addAction (actions.act ("erasechip"));
    tpicops->addAction (actions.act ("burnchip"));
    tpicops->addAction (actions.act ("verifychip"));

    QMenu *extra = new QMenu ("&Extras", this);
    menuBar ()->addMenu (extra);
    connect (extra, SIGNAL (aboutToShow ()), this, SLOT (slotExtraMenuManagement ()));

    /* ----------------------------------------------------------------------- 
       widget central
       ----------------------------------------------------------------------- */
    QWidget *central = new QWidget (this);
    setCentralWidget (central);
    QGridLayout *Lcentral = new QGridLayout (central);

    // default strech factor is 1, excepted for the last row
    Lcentral->setRowStretch (0, 1);
    Lcentral->setRowStretch (1, 1);
    Lcentral->setRowStretch (2, 1);
    Lcentral->setRowStretch (3, 1);
    Lcentral->setRowStretch (4, 1);
    Lcentral->setRowStretch (5, 0); // reserve row

    // display current settings
    QGroupBox *settingbox = new QGroupBox (tr ("Current settings"), central);
    QHBoxLayout *Lsettingbox = new QHBoxLayout (settingbox);

    QLabel *pname = new QLabel (curpic->name (), settingbox);

    QPalette pal = pname->palette ();
    pal.setColor (QPalette::WindowText, QColor (255, 0, 0));
    pname->setPalette (pal);

    Lsettingbox->addWidget (pname);

    settingbox->setMaximumHeight (settingbox->sizeHint ().height () + 8);

    fileNameLabel = new QLineEdit (settingbox);
    Lsettingbox->addWidget (fileNameLabel);
    fileNameLabel->setReadOnly (true);
    setFileName (fileName);

    Lcentral->addWidget (settingbox, 0, 0, 1, 2);

    // init hook for progress bar management
    curpic->setProgressListener (this);

    curpic->setProgOptionNumber (-1);

    // display programming options, if any
    if (*curpic->progOptions ())
      {
        memdisplay_h -= 3; // save space
        QGroupBox *optbox = new QGroupBox (tr ("Programming options"), central);
        QVBoxLayout *Loptbox = new QVBoxLayout (optbox);
        QComboBox *progopt = new QComboBox (optbox);
        Loptbox->addWidget (progopt);
        progopt->addItems (parseProgOptions (curpic->progOptions ()));
        slotProgOptionSelected (0);
        Lcentral->addWidget (optbox, 1, 0, 1, 2);
        connect (progopt, SIGNAL (activated (int)), this, SLOT (slotProgOptionSelected (int)));
      }
    else
      {
         Lcentral->setRowStretch (1, 0);
         Lcentral->setRowStretch (5, 1);
      }

    curpic->getBuffers (&mem, mem1, mem2, ee1, ee2, id1, id2, cf1, cf2,
			osccal_backup1, osccal_backup2);

    bits = QString (tr ("(%1 bits words)")).arg (curpic->wwidth ());

    MemoryDisplayer *md1 = new MemoryDisplayer (central, *mem, mem1, mem2 - mem1 + 1,
						memdisplay_h, 8U, tr ("Program memory ") + bits,
						curpic->wmask (), curpic->byteAdresses (),
						curpic->SPLocs ());
    Lcentral->addWidget (md1, 2, 0, 1, 2);

    connect (this, SIGNAL (bufferModified ()), md1, SLOT (updateDisplay ()));

    if (ee1 != -1)
      {
        QString mess = tr ("Data memory (8 bits words)");
        unsigned int mask = 0xFF;
        if (curpic->deviceCore () == 16)
	  {
            mess = tr ("Data memory (packed 8 bits words)");
            mask = 0xFFFF;
	  }
        MemoryDisplayer *md2 = new MemoryDisplayer (central, *mem, ee1, ee2 - ee1 + 1, 6U, 8U,
						    mess, mask, curpic->byteAdresses (),
						    curpic->SPLocs ());
        Lcentral->addWidget (md2, 3, 0, 1, 2);
        connect (this, SIGNAL (bufferModified ()), md2, SLOT (updateDisplay ()));
      }
    else
      {
         Lcentral->setRowStretch (3, 0);
         Lcentral->setRowStretch (5, 1);
      }

    if (id1 != -1)
      {
	unsigned int nbid = id2 - id1 + 1;
        unsigned int nbcol = nbid < 8 ? nbid : 8;
        unsigned int nbline = ((nbid / nbcol) > 1) ? 2 : 1;

        QString mess = tr ("ID data ") + bits;
        if (curpic->deviceCore () == 16)
	  mess = tr ("ID data (packed 8 bits words)");
        MemoryDisplayer *md3 = new MemoryDisplayer (central, *mem, id1, nbid, nbline, nbcol,
						    mess, curpic->wmask (),
						    curpic->byteAdresses (), curpic->SPLocs ());
        Lcentral->addWidget (md3, 4, 0, 1, 1);
        connect (this, SIGNAL (bufferModified ()), md3, SLOT (updateDisplay ()));
      }
    else
      {
	Lcentral->setRowStretch (4, 0);
	Lcentral->setRowStretch (5, 1);
      }

    // set col strch
    Lcentral->setColumnStretch (1, 10);
    Lcentral->setColumnStretch (0, 13);

    // right part - config bits
    // ----------------------
    QGridLayout *Lconf = new QGridLayout;
    Lcentral->addLayout (Lconf, 0, 2, 6, 1);

    BinWordsEditor *confuses = new BinWordsEditor (*mem, cf1, cf2, curpic->config_bits (),
						   tr ("Config Fuses"), curpic->byteAdresses (),
						   central);
    confuses->setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    Lconf->addWidget (confuses, 0, 0, 1, 1);
    connect (this, SIGNAL (bufferModified ()), confuses, SIGNAL (wordsChanged ()));

    // TODO adjust the bp of this display

    // display osccal backup
    if (osccal_backup1 != -1)
      {
	QString mess = tr ("Calibration");

        int nbo = osccal_backup2 - osccal_backup1 + 1;
        MemoryDisplayer *md4 = new MemoryDisplayer (central, *mem, osccal_backup1, nbo, nbo, 1,
						    mess, curpic->wmask (),
						    curpic->byteAdresses (), curpic->SPLocs ());
        Lconf->addWidget (md4, 1, 0, 1, 1);
        connect (this, SIGNAL (bufferModified ()), md4, SLOT (updateDisplay ()));
        // hack : allow extension in dummy last row
	Lcentral->setRowStretch (5, 1);
      }

    statBar = statusBar ();

    statBar->addWidget (new QLabel (" ", this));

    progRunning = new Indicator (this);
    statBar->addWidget (progRunning);
    statusLabel = new QLabel (this);
    statBar->addWidget (statusLabel);
    progress = new QProgressBar (this);
    progress->setRange (0, 100);
    statBar->addWidget (progress);

    // here we need an elastic space..
    statBar->addWidget (new QWidget (this), 1);

    statBar->addWidget (new QLabel (tr ("  Device:"), this));
    detected = new QLineEdit (this);
    detected->setReadOnly (true);
    statBar->addWidget (detected);

    statBar->addWidget (new QLabel (tr ("Rev:"), this));

    revdetected = new QLineEdit (this);
    revdetected->setText ("00");
    QFontMetrics fm (f);
    QRect rrev = fm.boundingRect (".WW.");
    revdetected->setFixedWidth (rrev.width ());
    revdetected->setReadOnly (true);

    statBar->addWidget (revdetected);
    statBar->addWidget (new QLabel ("  ", this)); // spacer

    QPushButton *detectButton = new QPushButton (tr ("Detect now"), this);

    detectButton->setFont (f);
    detectButton->updateGeometry ();
    detectButton->setToolTip (tr ("Probe the connected device."));
    statBar->addWidget (detectButton);

    connect (detectButton, SIGNAL (clicked ()), this, SLOT (slotDetectDeviceType ()));

    setStatus (false, QString (tr ("Ok")));

    setWindowIcon (QPixmap (":/src/icons/16/pikdev_icon.png"));
}

PrgWid::~PrgWid ()
{
    curpic->setProgressListener (0);
}

void PrgWid::go ()
{
    setMaximumHeight (sizeHint ().height ());
    show ();
}

void PrgWid::quit ()
{
    close ();
}

void PrgWid::saveHex ()
{
    const char *p;
    p = curpic->writeHexFile (fileName.toLatin1 (), "inhx32");
    if (p != 0)
      QMessageBox::critical (this, tr ("Sorry"), p, tr ("Ok"));
    else
      emit fileUpdated (fileName);
}

void PrgWid::saveHexAs ()
{
    QString fname = QFileDialog::getOpenFileName (this, "Select a file", ".",
						  "Intel hex files (*.hex *.HEX)");

    if (! fname.isEmpty ())
      {
        const char *p;
        p = curpic->writeHexFile (fname.toLatin1 (), "inhx32");
        if (p != 0)
	  QMessageBox::critical (this, tr ("Sorry"), p, tr ("Ok"));
        else
	  {
            emit fileUpdated (fname);
	    setFileName (fname);
	  }
      }
}

// reload
void PrgWid::loadHex ()
{
    const char *p;
    p = curpic->loadHexFile (fileName.toLatin1 ());
    if (p != 0)
      QMessageBox::critical (this, tr ("Sorry"), p, tr ("Ok"));
    else
      emit bufferModified ();
}

void PrgWid::loadHexAs ()
{
    QString fname = QFileDialog::getOpenFileName (this, "Select a file", ".",
						  "Intel hex files (*.hex *.HEX)");

    if (! fname.isEmpty ())
      {
        const char *p;
        p = curpic->loadHexFile (fname.toLatin1 ());
        if (p != 0)
	  QMessageBox::critical (this, tr ("Sorry"), p, tr ("Ok"));
        else
	  {
	    emit bufferModified ();
            setFileName (fname);
	  }
      }
}

void PrgWid::readPic ()
{
    if (! verifyDeviceType ())
      return;

    QString qtxt;
    emit messageGenerated (qtxt);
    setStatus (true, tr ("Reading ..."));
    QApplication::processEvents (QEventLoop::ExcludeUserInputEvents, 200);
    curpic->read_pic ();
    emit bufferModified ();
    setStatus (false, tr ("Ok"));
}

void PrgWid::erasePic ()
{
    if (! verifyDeviceType ())
      return;

    QString qtxt;

    emit messageGenerated (qtxt);
    setStatus (true, tr ("Erasing ..."));
    QApplication::processEvents (QEventLoop::ExcludeUserInputEvents, 200);
    int i = curpic->erase_pic ();
    if (i != 0)
      setStatus (false, curpic->error_msg (i));
    else
      setStatus (false, tr ("Ok"));
}

void PrgWid::verifyPic ()
{
    if (! verifyDeviceType ())
      return;

    QString qtxt = "";

    emit messageGenerated (qtxt);

    setStatus (true, tr ("Verifying ..."));
    QApplication::processEvents (QEventLoop::ExcludeUserInputEvents, 200);
    ostringstream out;

    int i = curpic->verify_pic (out);

    if (i != 0)
      {
        QString txt (out.str ().c_str ());
        emit messageGenerated (txt);
        setStatus (false, curpic->error_msg (i));
      }
    else
      setStatus (false, tr ("Ok"));
}

void PrgWid::programPic ()
{
    if (! verifyDeviceType ())
      return;

    QString qtxt;
    setStatus (true, QString (tr ("Programming ...")));
    emit messageGenerated (qtxt);

    QApplication::processEvents (QEventLoop::ExcludeUserInputEvents, 400);

    ostringstream out;
    int i = curpic->program_pic (out);

    if (i != 0)
      {
        qtxt = out.str ().c_str ();
        emit messageGenerated (qtxt);
        setStatus (false, curpic->error_msg (i));
      }
    else
      setStatus (false, tr ("Ok"));
}

void PrgWid::setStatus (bool active, const QString& txt)
{
    progRunning->setOn (active);

    statusLabel->setText (" " + txt + " ");
    QApplication::processEvents (QEventLoop::ExcludeUserInputEvents, 200);
}

// change filename - update UI
void PrgWid::setFileName (const QString& fname)
{
    fileName = fname;
    QString fn ((fname == "") ? tr ("<none>") : fname);
    fileNameLabel->setText (fn);
}

// Find the KAction "name" in the KMainWindow actionCollection ()
QAction *PrgWid::act (const char *name) const
{
    return actions.act (name);
}

void PrgWid::slotDetectDeviceType ()
{
    verifyDeviceType ();
}

/* ----------------------------------------------------------------------- 
   Verify the currently plugged device type

   Returns:
   true if detected device type is compatible with current pic model
   or if device type is not autodetectable
   ----------------------------------------------------------------------- */
bool PrgWid::verifyDeviceType ()
{
    QString dev;

    revdetected->setText ("");

    if (! curpic->isDetectable ())
      {
        detected->setText (tr ("<Non autodetectable device>"));
        return true;
      }

    dev = curpic->read_device_name ().c_str ();

    if (dev == "")
      {
        detected->setText (tr ("<No device detected>"));
        return false;
      }
    else if (dev == "?")
      {
        detected->setText (tr ("<Unknown device>"));
        return false;
      }
    detected->setText (dev);
    revdetected->setText (curpic->read_device_revision ().c_str ());

    bool ok = verifyDeviceType (dev);
    setStatus (false, ok ? tr ("Ok") : tr ("Mismatch"));
    return ok;
}

bool PrgWid::verifyDeviceType (const QString& type)
{
    if (type == curpic->specificName ())
      return true;

    // Not valid, ALERT
    int resp = QMessageBox::warning (this, tr ("Sorry"),
				     tr ("Detected device type does not match specified target device."),
				     tr ("Continue"), tr ("Abort"));
    return resp == 0;
}

bool PrgWid::queryClose ()
{
    return true;
}

QStringList PrgWid::parseProgOptions (const QString& op)
{
    QRegExp r ("\\s*\\{([^}]+)\\}");
    int index = 0;
    QStringList list;

    while (r.indexIn (op, index) != -1)
      {
        index += r.matchedLength ();
        list += r.cap (1);
      }
    return list;
}

void PrgWid::slotProgOptionSelected (int opt)
{
    curpic->setProgOptionNumber (opt);
}

void PrgWid::updateProgressBar (int percent)
{
    progress->setValue (percent);
    QApplication::processEvents (QEventLoop::ExcludeUserInputEvents, 200);
}

void PrgWid::updateProgressText (const char *text)
{
    statusLabel->setText (text);
    QApplication::processEvents (QEventLoop::ExcludeUserInputEvents, 200);
}

void PrgWid::slotExtraMenuManagement ()
{
    QString extra = programmer->extraFct ();
    QStringList extraList = parseProgOptions (extra);

    QStringList::iterator i;

    QMenu *pop = dynamic_cast<QMenu *>(sender ());

    pop->clear ();

    int nb;

    for (i = extraList.begin (), nb = 0; i != extraList.end (); ++i, ++nb)
      {
        QAction *a = pop->addAction (*i, this, SLOT (slotExecExtra (bool)));
        a->setData (QVariant (nb));
      }
}

void PrgWid::slotExecExtra (bool)
{
    QAction *act = dynamic_cast<QAction *>(sender ());
    if (act)
      programmer->executeExtraFct (act->data ().toInt ());
}
