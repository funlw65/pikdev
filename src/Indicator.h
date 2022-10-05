#ifndef INDICATOR_H
#define INDICATOR_H

#include <QLabel>
#include <QSize>
#include <QWidget>


class Indicator : public QLabel
{
    bool on;

public:
    Indicator (QWidget *);
    bool isOn ();
    void setOn (bool);
    virtual QSize sizeHint ();
};

#endif // INDICATOR_H
