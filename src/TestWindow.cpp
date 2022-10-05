#include <iostream>
using namespace std;

#include <QMainWindow>

#include "TestWindow.h"


TestWindow:: TestWindow (QWidget *w) : QMainWindow (w)
{
    setAttribute (Qt::WA_DeleteOnClose);
    setCentralWidget (new QWidget (this));
    show ();
}

TestWindow::~TestWindow ()
{
}
