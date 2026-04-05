#include "dbctool.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DbcTool w;
    w.show();
    return a.exec();
}
