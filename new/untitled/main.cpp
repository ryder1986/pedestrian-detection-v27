#include <QCoreApplication>
#include "test.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    test t;
    return a.exec();
}
