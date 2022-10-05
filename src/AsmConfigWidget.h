/* Assembler Configuration Widget */

#ifndef ASM_CONFIGWIDGET
#define ASM_CONFIGWIDGET

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QMainWindow>

#include "ConfigWidget.h"
#include "PikMain.h"
#include "Project.h"


class AsmConfigWidget : public ConfigWidget
{
    Q_OBJECT

private:
    // assembler configuration
    QComboBox *pr; // processors
    QComboBox *ff; // file formats
    QComboBox *ra; // Radix
    QComboBox *wa; // Warning levels
    QLineEdit *id; // Include directories
    QLineEdit *oo; // Other options
    // deassembler configuratioin
    QComboBox *dpr; // processors
    QLineEdit *doo; // Other options
    QCheckBox *dshort; // short linsting needed

    // the app widget
    PikMain *main_widget;
    // true if config widget is really used
    // (in project mode, config widget is not used)
    bool configurator_valid;
    
public:
    AsmConfigWidget ();
    // short name of configurator
    virtual QString configName ();
    // title string for this configurator
    virtual QString configTitle ();
    // return icon of this configurator
    virtual QPixmap icon ();
  
public slots:
    // apply configured data (ie: save them to config file)
    virtual void doApply ();
};

#endif

