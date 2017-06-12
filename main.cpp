#include "chatsystem.h"
#include <QApplication>
//#include <QtGui/QApplication>
//#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());

    ChatSystem w;
    w.show();

    return a.exec();
}
