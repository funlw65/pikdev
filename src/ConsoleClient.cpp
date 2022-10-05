#include <QApplication>

#include "ConsoleClient.h"
#include "PikMain.h"


ConsoleClient::ConsoleClient ()
{
    // cannot get console addr here, because main window not created yet ...
}

ConsoleClient::~ConsoleClient ()
{
}

void ConsoleClient::clearConsole ()
{
    PikMain *main = PikMain::mainWindow ();
    main->getConsole ()->clear ();
}

void ConsoleClient::addTextConsole (const QString& txt)
{
    PikMain *main = PikMain::mainWindow ();
    main->getConsole ()->append (txt);
    QApplication::processEvents ();
}
