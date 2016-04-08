#include "newuser.h"
#include "ui_newuser.h"

NewUser::NewUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewUser)
{
    ui->setupUi(this);
}

NewUser::~NewUser()
{
    delete ui;
}

void NewUser::on_backButton_clicked()
{
    this->close();
}

void NewUser::checkUserInfo() {
    if (ui->passwordconfirm->text() != "") {
        if (ui->passwordconfirm->text() != ui->password->text()) {
            ui->passwordLabel->setVisible(true);
            ui->forwardButton->setEnabled(false);
            return;
        } else {
            ui->passwordLabel->setVisible(false);
        }
    }

    if (ui->fullname->text() != "" && ui->loginname->text() != "" && ui->password->text() != "" && ui->passwordconfirm->text() != "") {
        ui->forwardButton->setEnabled(true);
    } else {
        ui->forwardButton->setEnabled(false);
    }
}

void NewUser::on_fullname_textChanged(const QString &arg1)
{
    if (arg1.split(" ").count() >= 1) {
        ui->loginname->setText(arg1.split(" ").at(0).toLower());
    }

    checkUserInfo();
}

void NewUser::on_password_textChanged(const QString &arg1)
{
    checkUserInfo();
}

void NewUser::on_passwordconfirm_textChanged(const QString &arg1)
{
    checkUserInfo();
}

void NewUser::on_loginname_textChanged(const QString &arg1)
{
    checkUserInfo();
}

void NewUser::on_forwardButton_clicked()
{
    QProcess* process = new QProcess();
    if (ui->checkBox->isChecked()) { //This is an administrative account. Add to wheel
        process->start("useradd -m -G wheel " + ui->loginname->text());
        process->waitForFinished(-1);
    } else {
        process->start("useradd -m " + ui->loginname->text());
        process->waitForFinished(-1);
    }

    process->start("chpasswd");
    process->write(QString(ui->loginname->text() + ":" + ui->password->text() + "\n").toUtf8());
    process->closeWriteChannel();
    process->waitForFinished(-1);
    this->close();
}
