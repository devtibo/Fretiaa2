#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //  === General Settings ===
    a.setApplicationVersion(QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));     // soft version);
    a.setApplicationName(QString("FReTiAA (") + a.applicationVersion() + QString("), A Free Real Time Audio Analyzer"));
    a.setApplicationDisplayName(a.applicationName() +  " -Created by Dr. Fux-");
    a.setWindowIcon(QIcon(":/icons/iconFretiaa.png"));

    // -  Apparence // source : http://www.yasinuludag.com/darkorange.stylesheet

    a.setStyle("plastique");
    QFile f(":/darkorange/darkorange.stylesheet");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    // === Main Window ===
    MainWindow w;
    w.show();
    // - Resize and center the window
    QScreen *screen = QGuiApplication::primaryScreen();
    w.resize(screen->availableGeometry().width()/2,screen->availableGeometry().height()/2);
    w.move(screen->availableGeometry().center() - w.rect().center());


    return a.exec();
}
