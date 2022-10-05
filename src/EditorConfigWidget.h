#ifndef EDITORCONFIGWIDGET_H
#define EDITORCONFIGWIDGET_H

#include <QCheckBox>
#include <QSpinBox>
#include <QFontComboBox>
#include <QComboBox>
#include <QGridLayout>
#include <QStackedLayout>

#include "ConfigWidget.h"


class EditorConfigWidget : public ConfigWidget
{
    Q_OBJECT

public:
    explicit EditorConfigWidget (QWidget *parent = 0);
    virtual ~EditorConfigWidget ();

public slots:
    virtual void doApply ();
    // change button's color
    void selectColor ();

public:
    // Return the name of the config widget
    virtual QString configName ();

    // Return the title of the config page
    virtual QString configTitle ();

    // Return the icon associated to the config page
    virtual QPixmap icon ();

private:
    QCheckBox *autoAlignText;
    QSpinBox *fontSize;
    QFontComboBox *fontSelector;
    // allows to find HL informations
    QStackedLayout *stack;

signals:

public slots:

};

#endif // EDITORCONFIGWIDGET_H
