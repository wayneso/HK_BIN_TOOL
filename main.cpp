#include "hk_bin_tool.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HK_BIN_Tool w;
    w.show();
    return a.exec();
}
