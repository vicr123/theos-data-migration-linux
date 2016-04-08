#include "transferprogress.h"
#include "ui_transferprogress.h"

TransferProgress::TransferProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransferProgress)
{
    ui->setupUi(this);

    ui->flag->setPixmap(QIcon::fromTheme("flag").pixmap(24,24));
    ui->endFrame->setVisible(false);
}

TransferProgress::~TransferProgress()
{
    delete ui;
}

void TransferProgress::setProgress(qulonglong value, qulonglong max) {
    ui->currentProgress->setMaximum(max);
    ui->currentProgress->setValue(value);
}

void TransferProgress::finished() {
    ui->currentProgress->setVisible(false);
    ui->endFrame->setVisible(true);
    ui->label->setText("The transfer has completed. Reboot your PC to finish the transfer.");
}

void TransferProgress::on_pushButton_clicked()
{
    QProcess::startDetached("shutdown -r now");
}
