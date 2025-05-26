#include <QApplication>
#include "maincppmuttest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainCppMutTest w;
    w.show();
    return a.exec();
}
