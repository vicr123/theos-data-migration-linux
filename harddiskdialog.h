#ifndef HARDDISKDIALOG_H
#define HARDDISKDIALOG_H

#include <QDialog>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QProcess>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QDir>
#include <QMap>
#include <QVariant>
#include "udisks2.h"
#include "countworker.h"
#include "newuser.h"
#include "transferprogress.h"
#include "storagemediaworker.h"

extern QString calculateSize(quint64 size);

namespace Ui {
class HardDiskDialog;
}

class HardDiskDialog : public QDialog
{
    Q_OBJECT

    enum paneTypes {
        Drive = 0,
        TransferPC = 1,
        Transfer = 2,
        Environment = 3,
        User = 4,
        Confirm = 5,
        DoTransfer = 6
    };

    paneTypes currentPane = Drive;

public:
    explicit HardDiskDialog(QWidget *parent = 0);
    ~HardDiskDialog();

private slots:
    void on_quitButton_clicked();

    void on_forwardButton_clicked();

    void on_backButton_clicked();

    void on_itemsToTransfer_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_driveList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_TransferList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void countWorkerDone(qulonglong size, QTreeWidgetItem* rootItem);

    void userSetupChangedIndex(int index);

    void on_backButton_2_clicked();

private:
    Ui::HardDiskDialog *ui;

    void switchToPane(paneTypes pane);
    void SetupDrivePane();
    void SetupTransferPcPane();
    void SetupTransferPane();
    void SetupEnvironmentPane();
    void SetupUserSetupPane();

    QDir root;
    QDir AllTransfersDir;

    QMap<QString, QString> userMapping;

    qulonglong transferSize;
    UDisks2* udisks;
};

#endif // HARDDISKDIALOG_H
