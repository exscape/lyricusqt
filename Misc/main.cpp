#include "UI/mainwindow.h"
#include "Misc/application.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    MainWindow w;
    w.adjustSize();

    w.show();
    w.adjustSize(); // TODO: is this required (again)? Qt docs mention this must be done AFTER show() on some platforms (not Windows, though)

    return a.exec();
}
