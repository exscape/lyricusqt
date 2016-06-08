#include "UI/mainwindow.h"
#include "Misc/application.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
