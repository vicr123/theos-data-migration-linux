#ifndef TRANSFERPROGRESS_H
#define TRANSFERPROGRESS_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class TransferProgress;
}

class TransferProgress : public QDialog
{
    Q_OBJECT

public:
    explicit TransferProgress(QWidget *parent = 0);
    ~TransferProgress();

public slots:
    void setProgress(qulonglong value, qulonglong max);
    void finished();

private slots:
    void on_pushButton_clicked();

private:
    Ui::TransferProgress *ui;
};

#endif // TRANSFERPROGRESS_H
