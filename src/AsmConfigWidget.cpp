#include <cstdio>
#include <iostream>
using namespace std;

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QToolTip>

#include "AsmConfigWidget.h"
#include "Config.h"
#include "PicFactory.hh"


AsmConfigWidget::AsmConfigWidget () : ConfigWidget (0)
{
    QString p, h, i, r, w, o;
    QString dp, dother;
    bool ds;
    int k;
    
    main_widget = PikMain::mainWindow ();

    QStringList procs = main_widget->getProcList ();
    QStringList hfmt;
    (((hfmt += "inhx8m") += "inhx8s") += "inhx16") += "inhx32";
    QStringList rad;
    (rad += "dec") += "hex";
    QStringList warn;
    ((warn +="0") += "1") += "2";

    // disable asm configurator in project mode
    if (main_widget && main_widget->projectWid ()->getProject ())
      {
	configurator_valid = false;
        QLabel *mess = new QLabel (tr ("<b><font color=#FF0000> A project is currently open</b></font><br><hr>"
				       "Please use the project configurator dialog <br> "
				       "(<i>project->edit project</i>) for project-specific "
				       "assembler configuration.<hr>"), this);
        addWidget (mess);
      }
    else
      {
        configurator_valid = true;

        // get config from config file
        Config::getAsmConfig (p, h, i, r, w, o);

        QGroupBox *asmbox = new QGroupBox (tr ("Assembler"), this);
        addWidget (asmbox);
        QGridLayout *Lasmbox = new QGridLayout (asmbox);
        Lasmbox->setMargin (15);

        // 1) Processor choice
        QLabel *labpr = new QLabel (asmbox);
	Lasmbox->addWidget (labpr, 0, 0);
        labpr->setText (tr ("Processor"));
        pr = new QComboBox (asmbox);
	Lasmbox->addWidget (pr, 0, 1);
        pr->addItem ("*"); // allows to use the device specified in sourcefile
        pr->addItems (procs);

        ((k = procs.indexOf (p)) == -1) && ((k = procs.indexOf (p + " (P)")) == -1);
        pr->setCurrentIndex (k + 1);
        pr->setToolTip (tr ("PiKdev can only program processors marked with (P).\n"
			    "For the development of new programming drivers, please see\n"
			    "http://pikdev.free.fr, or contact the author.\n"
			    "* means that processor is specified by PROCESSOR or LIST directive"));

        // 2) hex file format
        labpr = new QLabel (asmbox);
	Lasmbox->addWidget (labpr, 1, 0);
        labpr->setText (tr ("Hex file format"));
        ff = new QComboBox (asmbox);
	Lasmbox->addWidget (ff, 1, 1);
        ff->addItem ("*");
        ff->addItems (hfmt);
        k = hfmt.indexOf (h);
        ff->setCurrentIndex (k + 1);
        ff->setToolTip (tr ("CAUTION: inhx8s format is currently not supported by the PiKdev programmer.\n"
			    "If you need this support, please contact the author.\n"
			    "* means that format is specified by LIST directive"));

        // 3) radix
        labpr = new QLabel (asmbox);
	Lasmbox->addWidget (labpr, 2, 0);
        labpr->setText (tr ("Radix"));
        ra = new QComboBox (asmbox);
	Lasmbox->addWidget (ra, 2, 1);
        ra->setToolTip (tr ("Choose the default radix used by gpasm to parse integer constants\n"
			    "* means that radix is specified by LIST directive"));
        ra->addItem ("*");
        ra->addItems (rad);
        k = rad.indexOf (r);
        ra->setCurrentIndex (k + 1);

        // 4) Warning level
        labpr = new QLabel (asmbox);
	Lasmbox->addWidget (labpr, 3, 0);
        labpr->setText (tr ("Warning level"));
        wa = new QComboBox (asmbox);
	Lasmbox->addWidget (wa, 3, 1);
        wa->addItem ("*");
        wa->addItems (warn);
        k = warn.indexOf (w);
        wa->setCurrentIndex (k + 1);
        wa->setToolTip (tr ("For cleaner code, always prefer warning level 0.\n"
			    "* means that warning level is specified by LIST directive"));

        // 5) Include Directories
        labpr = new QLabel (asmbox);
	Lasmbox->addWidget (labpr, 4, 0);
        labpr->setText (tr ("Include directories"));
        id = new QLineEdit (asmbox);
	Lasmbox->addWidget (id, 4, 1);
        id->setText (i);
        id->setToolTip (tr ("By using the pseudovariable $(SRCPATH), you can specify an include directory\n"
			    "relative to the source file directory:\n"
			    "i.e.: compiling /home/alain/bar/x.asm with the include directory set to $(SRCPATH)/foo\n"
			    "will set the gpasm include directory to /home/alain/bar/foo"));

        // 6) other option
        labpr = new QLabel (asmbox);
	Lasmbox->addWidget (labpr, 5, 0);
        labpr->setText (tr ("Other options"));
        oo = new QLineEdit (asmbox);
	Lasmbox->addWidget (oo, 5, 1);
        oo->setText(o);
        oo->setToolTip (tr ("This item allows you to manually specify a gpasm \"exotic\" option.\n"
			    "The $(SRCPATH) pseudo variable if supported in this item as it is "
			    "in the \"Include Directories\" one."));
        asmbox->setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
      }
    // END of assembler groupbox

    // gpdasm group box
    QStringList dprocs =  main_widget->getDProcList ();
    Config::getDAsmConfig (dp, ds, dother);

    QGroupBox *dasmbox = new QGroupBox (tr ("Disassembler"), this);
    addWidget (dasmbox);
    QGridLayout *Ldasmbox = new QGridLayout (dasmbox);
    Ldasmbox->setMargin (15);

    // A) processor list
    QLabel *dlabpr = new QLabel (dasmbox);
    Ldasmbox->addWidget (dlabpr, 0, 0);
    dlabpr->setText (tr ("Processor"));
    dpr = new QComboBox (dasmbox);
    Ldasmbox->addWidget (dpr, 0, 1);
    dpr->addItems (dprocs);
    k = dprocs.indexOf (dp); // -1 if not found
    if (k == -1)
      k = 0;
    dpr->setCurrentIndex (k);
    dpr->setToolTip (tr ("Choose here the target processor"));

    // B) short option
    QLabel *dasml = new QLabel (dasmbox);
    Ldasmbox->addWidget (dasml, 1, 0);
    dasml->setText (tr ("Plain source code"));
    dshort = new QCheckBox (dasmbox);
    Ldasmbox->addWidget (dshort, 1, 1);
    dshort->setChecked (ds);
    dshort->setToolTip (tr ("Output a source file reassemblable by gpasm"));

    // C) Custom options
    QLabel *customl = new QLabel (dasmbox);
    Ldasmbox->addWidget (customl, 2, 0);
    customl->setText (tr ("Other options"));
    doo = new QLineEdit (dasmbox);
    Ldasmbox->addWidget (doo, 2, 1);
    doo->setText (dother);
    doo->setToolTip (tr ("This item allows you to manually specify a gpdasm \"exotic\" option."));

    dasmbox->setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    // END of gpdasm configuration
    show ();
}

void AsmConfigWidget::doApply ()
{
    // update asm config if not in project mode
    if (configurator_valid)
      {
        // remove the (P) suffix
        QString proc (pr->currentText ());
        if (proc.right (4) == " (P)")
	  proc.remove (proc.length () - 4, 4);
        Config::setAsmConfig (proc,
			      ff->currentText (),
			      id->text (),
			      ra->currentText (),
			      wa->currentText (),
			      oo->text ());
      }
    Config::setDAsmConfig (dpr->currentText (),
			   dshort->isChecked (),
			   doo->text ());
}

QString AsmConfigWidget::configName ()
{
    return tr ("Assembler");
}

QString AsmConfigWidget::configTitle ()
{
    return tr ("Assembler configuration");
}

QPixmap AsmConfigWidget::icon ()
{
    return QPixmap (":/src/icons/32/pikdev_config_asm.png");
}
