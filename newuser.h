#ifndef NEWUSER_H
#define NEWUSER_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class NewUser;
}

class NewUser : public QDialog
{
    Q_OBJECT

public:
    explicit NewUser(QWidget *parent = 0);
    ~NewUser();

private slots:
    void on_backButton_clicked();

    void on_fullname_textChanged(const QString &arg1);

    void checkUserInfo();
    void on_password_textChanged(const QString &arg1);

    void on_passwordconfirm_textChanged(const QString &arg1);

    void on_loginname_textChanged(const QString &arg1);

    void on_forwardButton_clicked();

private:
    Ui::NewUser *ui;
};

#endif // NEWUSER_H
