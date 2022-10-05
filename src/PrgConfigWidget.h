// Internal scheduler configuration widget
#ifndef PRG_CONFIGWIDGET
#define PRG_CONFIGWIDGET

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSet>
#include <QSlider>
#include <QTimer>

#include "ConfigWidget.h"
#include "hardware.hh"


class PrgConfigWidget : public ConfigWidget
{
    Q_OBJECT

private:
    hardware *phard;
    QComboBox *dev;
    QComboBox *vpp;
    QComboBox *vdd;
    QComboBox *clock;
    QComboBox *din;
    QComboBox *dout;
    QComboBox *drw;
    
    QCheckBox *ivpp;
    QCheckBox *ivdd;
    QCheckBox *iclock;
    QCheckBox *idin;
    QCheckBox *idout;
    QCheckBox *idrw;

    QCheckBox *cbvpp;
    QCheckBox *cbvdd;
    QCheckBox *cbclock;
    QCheckBox *cbdatain;
    QCheckBox *cbdataout;
    QCheckBox *cbrw;

    /* This list  must be built before any port allocation
       by calling the static function availableParports */
    static QStringList parports_;
    static QStringList serports_;

    QGroupBox *groupverif;
    QPushButton *bstopverify;
    QPushButton *bsendbits;
    QTimer *timersendbits, *timerpolldataout;

    QRadioButton *butpar, *butser;
    QButtonGroup *radiobuttons;
    QComboBox *parportlist, *serportlist;
    QSlider *delay_slider;

    void reconfigCombos (port* aport, int vp, int vd, int cl, int out, int in,
			 int rw, int clkdelay);
    void send_bits (unsigned int d, int nbb);

    QComboBox *stdh;
    QPushButton *stdsav;
    // A dummy programmer name which acts as a combobox title
    const QString null_prog_name;

public:
    PrgConfigWidget (hardware *h);
    static void availablePorts ();

    /* -----------------------------------------------------------------------
       Search the pin number pin in the string list list.

       Returns:
       The index of the found pin, or -1 if not found
       ----------------------------------------------------------------------- */
    static int findPin (const QStringList& list, int pin);

    /* Convert a pin name to pin number
       (a pin name must be a numeric decimal string  eventually followed by a
       blank and an arbitrary text) */
    static int pinNumber (const QString& pinname);

    // Extract the pin name from the Pin number + pin name string
    static const QString pinName (const QString& pinname);

    virtual QString configName ();
    virtual QString configTitle ();
    virtual QPixmap icon ();

public slots:

    virtual void doApply ();
    void driverSelected (int);

    // Continuously send bits to programmer board
    void sendBits ();

    void slotvpp ();
    void slotvdd ();
    void slotclock ();
    void slotdataout ();
    void slotrw ();
    void slotrwchange (const QString& string);
    void slotupdatedatain ();
    void slotSendBits ();
    // Changes pin/port  configuration when a new standard hardware is selected
    void slotStdHardware (const QString& name);
    // Creates a new hardware profile
    void slotSaveHardware ();

signals:
    void PrgConfigChanged ();
};

#endif
