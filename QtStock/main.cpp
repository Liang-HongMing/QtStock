#include "stockwin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StockWin w;
    w.show();
    return a.exec();
}
