#include "mainwindow.h"
#include "ui_mainwindow.h"

QString calculateSize(quint64 size) {
    QString ret;
    if (size > 1073741824) {
        ret = QString::number(((float) size / 1024 / 1024 / 1024), 'f', 2).append(" GiB");
    } else if (size > 1048576) {
        ret = QString::number(((float) size / 1024 / 1024), 'f', 2).append(" MiB");
    } else if (size > 1024) {
        ret = QString::number(((float) size / 1024), 'f', 2).append(" KiB");
    } else {
        ret = QString::number((float) size, 'f', 2).append(" B");
    }

    return ret;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);


    QStringList args = QApplication::arguments();
    args.removeAt(0);

    if (args.contains("--no-root")) {
        ui->noRootFrame->setVisible(false);
    } else {
        QFile shadow("/etc/shadow");
        shadow.open(QFile::ReadOnly);

        if (shadow.isReadable()) {
            ui->noRootFrame->setVisible(false);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_2_clicked()
{
    HardDiskDialog d;
    this->hide();
    d.exec();
    this->show();
}

void MainWindow::on_pushButton_4_clicked()
{
    this->hide();

    QFile paletteFile(QDir::home().filePath(".theos_palette"));
    paletteFile.open(QFile::ReadWrite);
    QPalette pal = QApplication::palette();
    paletteFile.write(QIcon::themeName().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Window).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::WindowText).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Button).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::ButtonText).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Base).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Disabled, QPalette::Window).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Disabled, QPalette::WindowText).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Disabled, QPalette::Button).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Disabled, QPalette::ButtonText).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Disabled, QPalette::Base).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Highlight).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::HighlightedText).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Text).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Active, QPalette::Light).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Active, QPalette::Midlight).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Active, QPalette::Mid).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Active, QPalette::Dark).name().toUtf8() + "\n");
    paletteFile.write(pal.color(QPalette::Active, QPalette::Shadow).name().toUtf8() + "\n");

    paletteFile.close();


    int retcode = QProcess::execute("kdesu --noignorebutton -d -c \"" + QCoreApplication::applicationFilePath() + " -p \'" + paletteFile.fileName() + "\'\"");

    paletteFile.remove();

    if (retcode == -1) {
        this->show();
    } else {
        QApplication::exit(retcode);
    }
}
