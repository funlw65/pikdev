#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QMainWindow>
#include <QWidget>

#include "codeeditor/QBEditor.h"


class TestWindow : public QMainWindow
{
    Q_OBJECT

private:
  
public:
    TestWindow (QWidget *w);
    virtual ~TestWindow ();

public slots:
};

#endif
