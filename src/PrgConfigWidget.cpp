#include <cassert>
#include <cstdio>
#include <cstdio>
#include <iostream>
using namespace std;

#include <QButtonGroup>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QSpinBox>
#include <QToolTip>

#include "Config.h"
#include "PrgConfigWidget.h"
#include "hardware.hh"
#include "serport.hh"
#include "uparport.hh"


QStringList PrgConfigWidget::parports_;

QStringList PrgConfigWidget::serports_;

PrgConfigWidget::PrgConfigWidget (hardware *hrd):
    ConfigWidget (0), phard (hrd),
    null_prog_name (tr ("<Select here a predefined configuration>"))
{
    QString type, device;
    int vp, vd, cl, out, in, rw, clkdelay;
    port *current_port = 0;

    QIcon iconset (":/src/codeeditor/icons/save.png");

    // QStringList devlist = getParportList ();
    static QString invPin (tr ("Check this option if your hardware uses negative logic for this pin"));
    static QString testpin (tr ("Check this box to turn voltage on/off for this pin"));

    // load the current config
    Config::getCurrentPortConfig (type, device, vp, vd, cl, out, in, rw, clkdelay);

    // 0) Standard programmers
    QGroupBox *stdbox = new QGroupBox (tr ("Standard cards"), this);
    addWidget (stdbox);
    QGridLayout *Lstdbox = new QGridLayout (stdbox);
    Lstdbox->setMargin (15);

    stdh = new QComboBox (stdbox); Lstdbox->addWidget (stdh, 0, 0);
    stdh->setToolTip (tr ("Select here a standard configuration.\n"
			  "You can edit the configuration name and save your own configuration.\n"
			  "Build-in standard configurations cannot be overwritten."));
    stdh->addItem (null_prog_name);
    stdh->addItems (Config::getStdHardwares ());
    stdh->setEditable (true);
    stdsav = new QPushButton (tr ("Save"), stdbox);
    Lstdbox->addWidget (stdsav, 0, 1);
    stdsav->setIcon (iconset);
    stdsav->setToolTip (tr ("Add the current configuration to the standard configurations list"));

    Lstdbox->setColumnStretch (0, 8);
    Lstdbox->setColumnStretch (1, 1);

    stdbox->setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);


    QString port_toolt_tip (tr ("Select here the port type you need.\n"
				" (Don't forget to click <Apply> or <Ok> to validate your selection)"));
    // 1) Port choice
    QGroupBox *devbox = new QGroupBox (tr ("Port and driver selection"), this);
    addWidget (devbox);
    QGridLayout *Ldevbox = new QGridLayout (devbox);
    Ldevbox->setMargin (15);

    butpar = new QRadioButton (devbox);
    Ldevbox->addWidget (butpar, 0, 0);
    butpar->setToolTip (port_toolt_tip);
    butpar->setText (tr ("Parallel"));

    butser = new QRadioButton (devbox);
    Ldevbox->addWidget (butser, 0, 1);
    butser->setText (tr ("Serial"));
    butser->setToolTip (port_toolt_tip);

    radiobuttons = new QButtonGroup (devbox);
    radiobuttons->addButton (butpar, 0);
    radiobuttons->addButton (butser, 1);

    parportlist = new QComboBox (devbox);
    Ldevbox->addWidget (parportlist, 1, 0);

    parportlist->addItems (parports_);
    parportlist->setToolTip (tr ("Available ports list:\n"
				 "If this list is empty, your machine might not have any parallel port\n"
				 "or the /dev/parportX device has not been created.\n"
				 "use \"mknod /dev/parport0 c 99 0\" to create it.\n"
				 "use \"chmod a+rw /dev/parport0\" to make it RW enabled."));

    parportlist->setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    serportlist = new QComboBox (devbox);
    Ldevbox->addWidget (serportlist, 1, 1);
    serportlist->addItems (serports_);
    serportlist->setToolTip (tr ("Available ports list:\n"
				 "If this list is empty, your machine might not have any serial port"));
    serportlist->setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    devbox->setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    // 2) GroupBox for pins assignment
    QGroupBox *group = new QGroupBox (tr ("Pin assignment"), this);
    addWidget (group);
    QGridLayout *Lgroup = new QGridLayout (group);
    Lgroup->setMargin (15);

    Lgroup->addWidget (new QLabel (tr ("Pin #"), group), 0, 1);
    Lgroup->addWidget (new QLabel (tr ("Inverted"), group), 0, 2);
    QLabel *lpin = new QLabel (tr ("Pin test"));
    lpin->setToolTip (tr ("Click Apply to validate you configuration,"
			  " then try it with the above checkboxes"));
    Lgroup->addWidget (lpin, 0, 3);

    // 2-1) Vpp pin
    Lgroup->addWidget (new QLabel ("Vpp", group), 1, 0);
    vpp = new QComboBox (group);
    Lgroup->addWidget (vpp, 1, 1);
    vpp->setToolTip (tr ("The VPP pin is used to select the high voltage programming mode."));
    ivpp = new QCheckBox (group);
    Lgroup->addWidget (ivpp, 1, 2);
    ivpp->setToolTip (invPin);
    cbvpp = new QCheckBox ("0 V", group);
    Lgroup->addWidget (cbvpp, 1, 3);
    cbvpp->setToolTip (testpin);
    connect (cbvpp, SIGNAL (clicked ()), this, SLOT (slotvpp ()));

    // 2-2) Vdd pin
    Lgroup->addWidget (new QLabel ("Vdd", group), 2, 0);
    vdd = new QComboBox (group);
    Lgroup->addWidget (vdd, 2, 1);
    vdd->setToolTip (tr ("The VDD pin is used to apply 5V to the programmed device.\nMust be set to NA if your programmer doesn't control the VDD line."));
    ivdd = new QCheckBox (group);
    Lgroup->addWidget (ivdd, 2, 2);
    ivdd->setToolTip (invPin);
    cbvdd = new QCheckBox ("0 V", group);
    Lgroup->addWidget (cbvdd, 2, 3);
    cbvdd->setToolTip (testpin);
    connect (cbvdd, SIGNAL (clicked ()), this, SLOT (slotvdd ()));

    // 2-3) Clock pin
    Lgroup->addWidget (new QLabel ("Clock", group), 3, 0);
    clock = new QComboBox (group);
    Lgroup->addWidget (clock, 3, 1);
    clock->setToolTip (tr ("The CLOCK pin is used to synchronize serial"
                           " data of the DATA IN and DATA OUT pins."));
    iclock = new QCheckBox (group);
    Lgroup->addWidget (iclock, 3, 2);
    iclock->setToolTip (invPin);
    cbclock = new QCheckBox ("0 V", group);
    Lgroup->addWidget (cbclock, 3, 3);
    cbclock->setToolTip (testpin);
    connect (cbclock, SIGNAL (clicked ()), this, SLOT (slotclock ()));

    // 2-4) Data in pin
    Lgroup->addWidget (new QLabel ("Data in", group), 4, 0);
    din = new QComboBox (group);
    Lgroup->addWidget (din, 4, 1);
    din->setToolTip (tr ("The DATA IN pin is used to receive data from the programmed device"));
    idin = new QCheckBox (group);
    Lgroup->addWidget (idin, 4, 2);
    idin->setToolTip (invPin);
    cbdatain = new QCheckBox ("0 V", group);
    Lgroup->addWidget (cbdatain, 4, 3);
    cbdatain->setToolTip (tr ("This pin is driven by the programmed device\n"
                              "Without device, it must follow the \"Data out\" pin (if power on) "));
    cbdatain->setEnabled (false);

    // 2-5) Data out pin
    Lgroup->addWidget (new QLabel ("Data out", group), 5, 0);
    dout = new QComboBox (group);
    Lgroup->addWidget (dout, 5, 1);
    dout->setToolTip (tr ("The DATA OUT pin is used to send data to the programmed device"));
    idout = new QCheckBox (group);
    Lgroup->addWidget (idout, 5, 2);
    idout->setChecked (out < 0);
    idout->setToolTip (invPin);
    cbdataout = new QCheckBox ("0 V", group);
    Lgroup->addWidget (cbdataout, 5, 3);
    connect (cbdataout, SIGNAL (clicked ()), this, SLOT (slotdataout ()));
    cbdataout->setToolTip (testpin);

    // 2-6) Data RW pin
    Lgroup->addWidget (new QLabel ("Data R/W", group), 6, 0);
    drw = new QComboBox (group);
    Lgroup->addWidget (drw, 6, 1);
    drw->setToolTip (tr ("The DATA RW pin selects the direction of data buffer.\nMust be set to NA if your programmer does not use bi-directionnal buffer."));
    connect (drw, SIGNAL (activated (const QString&)), this, SLOT (slotrwchange (const QString&)));
    idrw = new QCheckBox (group);
    Lgroup->addWidget (idrw, 6, 2);
    idrw->setChecked (rw < 0);
    idrw->setToolTip (invPin);
    cbrw = new QCheckBox ("Data In", group);
    Lgroup->addWidget (cbrw, 6, 3);
    connect (cbrw, SIGNAL (clicked ()), this, SLOT (slotrw ()));
    cbrw->setToolTip (tr ("Check this box to change DATA buffer direction"));

    // 3) "Verify config" button
    Lgroup->addWidget (new QLabel (tr ("Clock delay"), group), 7, 0);
    delay_slider = new QSlider (Qt::Horizontal, group);
    delay_slider->setMinimum (0);
    delay_slider->setMaximum (50);
    delay_slider->setPageStep (0);
    delay_slider->setValue (0);
    Lgroup->addWidget (delay_slider, 7, 1);
    QSpinBox *delay_spinbox = new QSpinBox (group);
    delay_spinbox->setMinimum (0);
    delay_spinbox->setMaximum (50);
    delay_slider->setSingleStep (1);

    Lgroup->addWidget (delay_spinbox, 7, 2);
    delay_slider->setValue (clkdelay);
    delay_spinbox->setValue (clkdelay);

    delay_spinbox->setToolTip (tr ("Some programming cards need low clock rate:\n"
				   "adding delay to clock pulses might help."));
    delay_slider->setToolTip (tr ("Some programming cards need low clock rate:\n"
				  "adding delay to clock pulses might help."));

    bsendbits = new QPushButton (tr ("Send 0xA55A"), group);
    Lgroup->addWidget (bsendbits, 7, 3);
    connect (bsendbits, SIGNAL (clicked ()), this, SLOT (sendBits ()));
    bsendbits->setToolTip (tr ("Continuously send 0xA55A on \"Data out\" pin"));

    group->setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    connect (delay_slider, SIGNAL (valueChanged (int)), delay_spinbox, SLOT (setValue (int)));
    connect (delay_spinbox, SIGNAL (valueChanged (int)), delay_slider, SLOT (setValue (int)));

    // add a filler to push widgets to top of dialog
    QWidget *filler = new QWidget (this);
    addWidget (filler);

    setMinimumWidth (sizeHint ().width () + 50);
    if (phard && phard->isok ())
      {
        // set all the pins low
        phard->clklo ();
        phard->outlo ();
        phard->vppoff ();
        phard->vddoff ();
        phard->rwread ();
      }

    // create Timer for sending bits
    timersendbits = new QTimer (this);
    connect (timersendbits, SIGNAL (timeout ()), this, SLOT (slotSendBits ()));

    // create timer for automatically poll dataout pin
    timerpolldataout = new QTimer (this);
    connect (timerpolldataout, SIGNAL (timeout ()), this, SLOT (slotupdatedatain ()));
    timerpolldataout->setSingleShot (false);
    timerpolldataout->start (100);

    connect (radiobuttons, SIGNAL (buttonClicked (int)), this, SLOT (driverSelected (int)));

    if (type == "parallel")
      butpar->setChecked (true), current_port = new parport;
    else if (type == "serial")
      butser->setChecked (true), current_port = new serport;

    connect (stdh, SIGNAL (activated (const QString&)), this,
	     SLOT (slotStdHardware (const QString&)));
    connect (stdsav, SIGNAL (clicked ()), this, SLOT (slotSaveHardware ()));

    // the port is just used by reconfigCombos a pin number provider
    if (current_port)
      reconfigCombos (current_port, vp, vd, cl, out, in, rw, clkdelay);
    delete current_port;

    show ();
}

/* ask OS for valid an available parports  (static method)
   must be called before any port allocation because an
   allocated parport is not seen as available
   I do not use anymore the "ls /dev/parportX" method
   because it reports ALL the existing /dev/parport inodes
   (and not only the physically existing ones) */
void PrgConfigWidget::availablePorts ()
{
    char name[100];
#if 1
    // standard parport in user space
    for (int i = 0; i < 8; ++i)
      {
        sprintf (name, "/dev/parport%d", i);
        if (parport::probePort (name)== 0) // available ?
	  parports_ += name;
      }
    // new devfs parport flavour
    for (int i = 0; i < 8; ++i)
      {
        sprintf (name, "/dev/parports/%d", i);
        if (parport::probePort (name) == 0) // available ?
	  parports_ += name;
      }
#else
    parports_ += "/dev/parport0";
#endif

    // standard serport in user space
    for (int i = 0; i < 8; ++i)
      {
        sprintf (name, "/dev/ttyS%d", i);
        if (serport::probePort (name) == 0) // available ?
	  serports_ += name;
      }
    // new devfs serport flavour
    for (int i = 0; i < 8; ++i)
      {
        sprintf (name, "/dev/tts/%d", i);
        if (serport::probePort (name) == 0) // available ?
	  serports_ += name;
      }
    // standard USB serport in user space
    for (int i = 0; i < 8; ++i)
      {
        sprintf (name, "/dev/ttyUSB%d", i);
        if (serport::probePort (name) == 0) // available ?
	  serports_ += name;
      }
    // new devfs USB serport flavour
    for (int i = 0; i < 8; ++i)
      {
        sprintf (name, "/dev/usb/tts/%d", i);
        if (serport::probePort (name) == 0) // available ?
	  serports_ += name;
      }
}

void PrgConfigWidget::doApply ()
{
    int vp = pinNumber (vpp->currentText ());
    if (ivpp->isChecked ())
      vp = -vp;

    int vd = pinNumber (vdd->currentText ());
    if (ivdd->isChecked ())
      vd = -vd;

    int clo = pinNumber (clock->currentText ());
    if (iclock->isChecked ())
      clo = -clo;

    int di = pinNumber (din->currentText ());
    if (idin->isChecked ())
      di = -di;

    int dou = pinNumber (dout->currentText ());
    if (idout->isChecked ())
      dou = -dou;

    int rw = pinNumber (drw->currentText ());
    if (idrw->isChecked ())
      rw = -rw;

    QString type, port;
    if (butpar->isChecked ())
      {
        type = "parallel";
        port = parportlist->currentText ();
      }
    else if (butser->isChecked ())
      {
        type = "serial";
        port = serportlist->currentText ();
      }

    int clkdelay = delay_slider->value ();
    // blank port == port unavailable
    if (port != "")
      {
        Config::setPortConfig (type, port, vp, vd, clo, dou, di, rw, clkdelay);
        phard->setpins (type.toLatin1 (), port.toLatin1 (), vp, vd, clo, dou, di, rw, clkdelay);
      }
    else
      {
        QMessageBox alert (QMessageBox::Warning, tr ("Sorry"), tr ("Port not available"),
			   QMessageBox::Ok, this);

        alert.setDetailedText (tr ("The selected port is not present on your system, or is not RW enabled.\n"
				   "Port selection refused."));
        alert.exec ();
      }

    // Reset status of pins graphically because inside phard->setpins () reset electrically...
    cbvpp->setText ("0 V");
    cbvpp->setChecked (false);
    cbvdd->setText ("0 V");
    cbvdd->setChecked (false);
    cbclock->setText ("0 V");
    cbclock->setChecked (false);
    cbdataout->setText ("0 V");
    cbdataout->setChecked (false);
    cbrw->setText ("Data In");
    cbrw->setChecked (false);

    emit PrgConfigChanged ();
}

void PrgConfigWidget::sendBits ()
{
    if (timersendbits->isActive ())
      {
        bsendbits->setText (tr ("Send 0xA55A"));
        timersendbits->stop ();
        slotdataout ();
      }
    else
      {
        bsendbits->setText (tr ("Stop"));
        timersendbits->setSingleShot (false);
        timersendbits->start (1);
      }
}

void PrgConfigWidget::slotrwchange (const QString& string)
{
    if (! phard->isok ())
      return;

    bool enable = pinName (string) != "(NA)";
    cbrw->setEnabled (enable);
    idrw->setEnabled (enable);
}

void PrgConfigWidget::slotvpp ()
{
    if (! phard->isok ())
      return;

    if (cbvpp->isChecked ())
      {
        cbvpp->setText ("13 V");
        phard->vppon ();
      }
    else
      {
        cbvpp->setText ("0 V");
        phard->vppoff ();
      }
    slotupdatedatain ();
}

void PrgConfigWidget::slotvdd ()
{
    if (! phard->isok ())
      return;

    if (cbvdd->isChecked ())
      {
        cbvdd->setText ("5 V");
        phard->vddon ();
      }
    else
      {
        cbvdd->setText ("0 V");
        phard->vddoff ();
      }
}

void PrgConfigWidget::slotclock ()
{
    if (! phard->isok ())
      return;

    if (cbclock->isChecked ())
      {
        cbclock->setText ("5 V");
        phard->clkhi ();
      }
    else
      {
        cbclock->setText ("0 V");
        phard->clklo ();
      }
}

void PrgConfigWidget::slotdataout ()
{
    if (! phard->isok ())
      return;

    if (cbdataout->isChecked ())
      {
        cbdataout->setText ("5 V");
        phard->outhi ();
      }
    else
      {
        cbdataout->setText ("0 V");
        phard->outlo ();
      }
}

void PrgConfigWidget::slotrw ()
{
    if (! phard->isok ())
      return;

    if (cbrw->isChecked ())
      {
        cbrw->setText ("Data Out");
        phard->rwwrite ();
      }
    else
      {
        cbrw->setText ("Data In");
        phard->rwread ();
      }
}

void PrgConfigWidget::slotupdatedatain ()
{
    if (! phard->isok ())
      return; // port not open/owned

    static int oldstate = 0;
    int state = phard->inbit ();

    if (state == oldstate)
      return;

    if (state)
      {
        cbdatain->setChecked (true);
        cbdatain->setText ("5 V");
      }
    else
      {
        cbdatain->setChecked (false);
        cbdatain->setText ("0 V");
      }
    oldstate = state;
}

void PrgConfigWidget::send_bits (unsigned int d, int nbb)
{
    if (phard && phard->isok ())
      {
        phard->rwwrite ();
        for(; nbb; --nbb)
	  {
            phard->clkhi (); /* Clock high */
            if (d & 0x01)
	      phard->outhi (); /* Output to 1 */
            else
	      phard->outlo (); /* Output to 0 */
            phard->clklo (); /* Clock lo */
            d >>= 1; /* Move the data over 1 bit */
	  }

        phard->outlo (); /* let data line in 0 state */
        phard->rwread ();
      }
}

void PrgConfigWidget::slotSendBits ()
{
    send_bits (0xA55A, 16);
}

/* ----------------------------------------------------------------------- 
   Search the pin number "pin" in the string list "list".

   Returns:
   The index of the found pin, or -1 if not found
   ----------------------------------------------------------------------- */
int PrgConfigWidget::findPin (const QStringList& list, int pin)
{
    QStringList::const_iterator i;
    int k;
    for (i = list.begin (), k = 0; i != list.end (); ++i, ++k)
      {
        if (pinNumber (*i) == pin)
	  return k;
      }
    return -1;
}

/* ----------------------------------------------------------------------- 
   Convert a pin name to pin number
   (a pin name must be a numeric decimal string eventually followed by a
   blank and an arbitrary text)

   Returns:
   A pin number, or -1 if the pinname is malformed
   ----------------------------------------------------------------------- */
int PrgConfigWidget::pinNumber (const QString& pinname)
{
    int pp = -1;
    sscanf (pinname.toLatin1 (), "%d", &pp);
    return pp;
}

void PrgConfigWidget::reconfigCombos (port *aport, int vp, int vd, int cl,
				      int out, int in, int rw, int clkdelay)
{
    int k, pin;

    //Lists of pin...
    QString plist = aport->outputPinsList ();
    QStringList outplist = plist.split (',');

    plist = aport->inputPinsList ();
    QStringList inplist = plist.split (',');

    // 2-1) Vpp pin
    vpp->clear ();
    vpp->addItems (outplist);
    pin = vp;
    if (pin < 0)
      pin = -pin;
    if ((k = findPin (outplist, pin)) == -1)
      k = 0;
    vpp->setCurrentIndex (k);
    ivpp->setChecked (vp < 0);

    // 2-2) Vdd pin
    vdd->clear ();
    vdd->addItems (outplist);
    pin = vd;
    if (pin < 0)
      pin = -pin;
    if ((k = findPin (outplist, pin)) == -1)
      k = 0;
    vdd->setCurrentIndex (k);
    ivdd->setChecked (vd < 0);

    // 2-3) Clock pin
    clock->clear ();
    clock->addItems (outplist);
    pin = cl;
    if (pin < 0)
      pin = -pin;
    if ((k = findPin (outplist, pin)) == -1)
      k = 0;
    clock->setCurrentIndex (k);
    iclock->setChecked (cl < 0);

    // 2-4) Data in pin
    din->clear ();
    din->addItems (inplist);
    pin = in;
    if (pin < 0)
      pin = -pin;
    if ((k = findPin (inplist, pin)) == -1)
      k = 0;
    din->setCurrentIndex (k);
    idin->setChecked (in < 0);

    // 2-5) Data out pin
    dout->clear ();
    dout->addItems (outplist);
    pin = out;
    if (pin < 0)
      pin = -pin;
    if ((k = findPin (outplist, pin)) == -1)
      k = 0;
    dout->setCurrentIndex (k);
    idout->setChecked (out < 0);

    // 2-5) Data out pin
    drw->clear ();
    drw->addItems (outplist);
    pin = rw;
    if (pin < 0)
      pin = -pin;
    if ((k = findPin (outplist, pin)) == -1)
      k = 0;
    drw->setCurrentIndex (k);
    idrw->setChecked (rw < 0);
    slotrwchange (drw->currentText ());

    // 2-6) clock delay
    delay_slider->setValue (clkdelay);
}

/* Changes pin/port configuration when a new standard hardware is selected */
void PrgConfigWidget::slotStdHardware (const QString& name)
{
    if (name == null_prog_name)
      return;
    // restore the initial item (which is a message)
    stdh->setCurrentIndex (0);

    QString type;
    int vpp, vdd, clock, dataout, datain, drw, clkdelay;
    port *aport;

    Config::getHardConfig (name, type, vpp, vdd, clock, dataout, datain, drw, clkdelay);

    if (type == "parallel")
      {
        aport = new parport;
        butpar->setChecked (true);
      }
    else if (type == "serial")
      {
        aport = new serport;
        butser->setChecked (true);
      }
    else
      return;

    reconfigCombos (aport, vpp, vdd, clock, dataout, datain, drw, clkdelay);

    delete aport;
}

void PrgConfigWidget::slotSaveHardware ()
{
    QString name = stdh->currentText ();
    if (name == null_prog_name)
      return;

    int vp = pinNumber (vpp->currentText ());
    if (ivpp->isChecked ())
      vp = -vp;

    int vd = pinNumber (vdd->currentText ());
    if (ivdd->isChecked ())
      vd = -vd;

    int clo = pinNumber (clock->currentText ());
    if (iclock->isChecked ())
      clo = -clo;

    int di = pinNumber (din->currentText ());
    if (idin->isChecked ())
      di = -di;

    int dou = pinNumber (dout->currentText ());
    if (idout->isChecked ())
      dou = -dou;

    int datarw = pinNumber (drw->currentText ());
    if (idrw->isChecked ())
      datarw = -datarw;

    QString type;
    if (butpar->isChecked ())
      type = "parallel";
    else if (butser->isChecked ())
      type = "serial";
    else
      type = "boot555";

    int clkdelay = delay_slider->value ();
    Config::setHardConfig (name, type, vp, vd, clo, dou, di, datarw, clkdelay);

    // update combo box
    stdh->clear ();
    stdh->addItem (null_prog_name);
    stdh->addItems (Config::getStdHardwares ());
    stdh->setCurrentIndex (0);
}

const QString PrgConfigWidget::pinName (const QString& pinname)
{
    char name[20];
    int k = sscanf (pinname.toLatin1 (), "%*d %19s", name);
    return (k == 1) ? name : "";
}

QString PrgConfigWidget::configName ()
{
    return tr ("Builtin Programmer");
}

QString PrgConfigWidget::configTitle ()
{
    return tr ("Builtin Programmer configuration");
}

QPixmap PrgConfigWidget::icon ()
{
    return QPixmap (":/src/icons/32/pikdev_config_prg.png");
}

void PrgConfigWidget::driverSelected (int drv)
{
    QString xport;
    int vp, vd, cl, out, in, k, rw, clkdelay;
    port *port = 0;
    parport pport;
    serport sport;

    if (drv == 0)
      {
        Config::getParPortConfig (xport, vp, vd, cl, out, in, rw, clkdelay);
        if ((k = parports_.indexOf (xport)) == -1)
	  k = 0;
        parportlist->setCurrentIndex (k);
        port = &pport;
      }
    else if (drv == 1)
      {
        Config::getSerPortConfig (xport, vp, vd, cl, out, in, rw, clkdelay);
        if ((k = serports_.indexOf (xport)) == -1)
	  k = 0;
        serportlist->setCurrentIndex (k);
        port = &sport;
      }
    if (port)
      reconfigCombos (port, vp, vd, cl, out, in, rw, clkdelay);
}
