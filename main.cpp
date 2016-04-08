#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList args = a.arguments();
    args.removeAt(0);

    bool getPaletteFile = false;
    for (QString arg : args) {
        if (getPaletteFile) {
            QFile paletteFile(arg);
            paletteFile.open(QFile::ReadOnly);
            QPalette pal = QApplication::palette();
            QStringList palette = QString(paletteFile.readAll()).split("\n");
            QIcon::setThemeName(palette[0]);
            pal.setColor(QPalette::Window, QColor(palette[1]));
            pal.setColor(QPalette::WindowText, QColor(palette[2]));
            pal.setColor(QPalette::Button, QColor(palette[3]));
            pal.setColor(QPalette::ButtonText, QColor(palette[4]));
            pal.setColor(QPalette::Base, QColor(palette[5]));
            pal.setColor(QPalette::Disabled, QPalette::Window, QColor(palette[6]));
            pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(palette[7]));
            pal.setColor(QPalette::Disabled, QPalette::Button, QColor(palette[8]));
            pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(palette[9]));
            pal.setColor(QPalette::Disabled, QPalette::Base, QColor(palette[10]));
            pal.setColor(QPalette::Highlight, QColor(palette[11]));
            pal.setColor(QPalette::HighlightedText, QColor(palette[12]));
            pal.setColor(QPalette::Text, QColor(palette[13]));
            pal.setColor(QPalette::Active, QPalette::Light, QColor(palette[14]));
            pal.setColor(QPalette::Active, QPalette::Midlight, QColor(palette[15]));
            pal.setColor(QPalette::Active, QPalette::Mid, QColor(palette[16]));
            pal.setColor(QPalette::Active, QPalette::Dark, QColor(palette[17]));
            pal.setColor(QPalette::Active, QPalette::Shadow, QColor(palette[18]));

            QApplication::setPalette(pal);
            paletteFile.close();
        } else {
            if (arg == "--palette-file" || arg == "-p") {
                getPaletteFile = true;
            } else if (arg == "--help" || arg == "-h") {
                qDebug() << "theOS Data Migration Tool";
                qDebug() << "Usage: theos_installer [OPTIONS]";
                qDebug() << "  -p, --palette-file           Use a palette file to draw colors";
                qDebug() << "      --no-root                Don't ask for root access";
                return 0;
            }
        }
    }

    MainWindow w;
    w.show();



    return a.exec();
}
