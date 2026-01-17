#include "qchartmain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QchartMain w;
    w.setBaseSize(800, 800);
    w.show();
    return a.exec();
}
