#include "harddiskdialog.h"
#include "ui_harddiskdialog.h"

HardDiskDialog::HardDiskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HardDiskDialog)
{
    ui->setupUi(this);

    udisks = new UDisks2(this);

    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    ui->flag->setPixmap(QIcon::fromTheme("flag").pixmap(24,24));
    ui->backButton->setVisible(false);

    switchToPane(Drive);
}

HardDiskDialog::~HardDiskDialog()
{
    delete udisks;
    delete ui;
}

void HardDiskDialog::on_quitButton_clicked()
{
    if (QMessageBox::question(this, "Cancel", "Do you want to cancel the transfer?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        this->close();
    }
}

void HardDiskDialog::switchToPane(paneTypes pane) {
    currentPane = pane;
    ui->progressThroughTransfer->setValue(pane);

    ui->DrivePane->setVisible(false);
    ui->TransferSelectionPane->setVisible(false);
    ui->TransferPane->setVisible(false);
    ui->EnvironmentPane->setVisible(false);
    ui->UserSetupPane->setVisible(false);
    ui->ReadyToTransferPane->setVisible(false);

    switch (pane) {
    case Drive:
        SetupDrivePane();
        ui->DrivePane->setVisible(true);
        break;
    case TransferPC:
        SetupTransferPcPane();
        ui->TransferSelectionPane->setVisible(true);
        break;
    case Transfer:
        SetupTransferPane();
        ui->TransferPane->setVisible(true);
        break;
    case Environment:
        SetupEnvironmentPane();
        ui->EnvironmentPane->setVisible(true);
        break;
    case User:
        SetupUserSetupPane();
        ui->UserSetupPane->setVisible(true);
        break;
    case Confirm:
        ui->ReadyToTransferPane->setVisible(true);
        break;
    case DoTransfer:
        TransferProgress* p = new TransferProgress(this);
        p->showFullScreen();

        QThread *thread = new QThread();
        StorageMediaWorker *worker = new StorageMediaWorker(root, userMapping, transferSize);
        worker->moveToThread(thread);
        connect(thread, SIGNAL (started()), worker, SLOT (startWork()));
        connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
        connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
        connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
        connect(worker, SIGNAL (progressChanged(qulonglong,qulonglong)), p, SLOT (setProgress(qulonglong,qulonglong)));
        connect(worker, SIGNAL (finished()), p, SLOT (finished()));
        thread->start();
    }

    if (pane >= 1) {
        ui->backButton->setVisible(true);
    } else {
        ui->backButton->setVisible(false);
    }
}

void HardDiskDialog::SetupUserSetupPane() {
    QLayoutItem* item;
    while ((item = ui->UsersFrame->layout()->takeAt(0)) != NULL) {
        delete item->widget();
        delete item;
    }

    QFile passwdFile("/etc/passwd");
    passwdFile.open(QFile::ReadOnly);
    QString shadowOutput(passwdFile.readAll());
    QMap<QString, QString> availableUsers;
    for (QString user : shadowOutput.split("\n")) {
        if (user.contains("/home")) {
            //availableUsers.append(user.split(":").at(0));
            if (user.split(":").at(4) == "") {
                availableUsers.insert(user.split(":").at(0), user.split(":").at(0));
            } else {
                availableUsers.insert(user.split(":").at(4).split(",").at(0), user.split(":").at(0));
            }

        }
    }
    passwdFile.close();

    for (QString user : userMapping.keys()) {
        QLabel* label = new QLabel();
        label->setText(user);

        QComboBox* box = new QComboBox();
        connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(userSetupChangedIndex(int)));

        for (QString systemUser : availableUsers.keys()) {
            QStringList userData;
            userData << user << availableUsers.value(systemUser);
            QVariant data(userData);
            box->addItem(systemUser, data);
        }

        QStringList userData;
        userData << user << "nobody";
        QVariant data(userData);
        box->addItem("Don't transfer", data);

        ((QFormLayout*) ui->UsersFrame->layout())->addRow(label, box);

    }
}

void HardDiskDialog::userSetupChangedIndex(int index) {
    QComboBox* box = (QComboBox*) sender();
    QStringList userData = box->itemData(index).toStringList();
    userMapping.insert(userData.at(0), userData.at(1));
}

void HardDiskDialog::SetupEnvironmentPane() {
    bool blockNext = false;
    QProcess* df = new QProcess(this);
    df->start("df -B1 --output=avail /");
    df->waitForFinished(-1);
    qulonglong diskSize = QString(df->readAll()).split("\n").at(1).toULongLong();
    if (diskSize < this->transferSize) {
        ui->SpaceLabel->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(22));
        blockNext = true;
    } else {
        ui->SpaceLabel->setPixmap(QIcon::fromTheme("dialog-ok").pixmap(22));
    }

    bool battery;
    QFile *adapter = new QFile("/sys/class/power_supply/AC/online");
    if (!adapter->exists()) {
        adapter = new QFile("/sys/class/power_supply/ADP1/online");
    }

    adapter->open(QFile::ReadOnly);
    QByteArray info = adapter->read(1);
    adapter->close();
    if (QString(info).startsWith("0")) {
        battery = false;
    } else {
        battery = true;
    }

    if (battery) {
        ui->PowerLabel->setPixmap(QIcon::fromTheme("dialog-ok").pixmap(22));
    } else {
        ui->PowerLabel->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(22));
    }

    if (blockNext) {
        ui->forwardButton->setEnabled(false);
    } else {
        ui->forwardButton->setEnabled(true);
    }
}

void HardDiskDialog::SetupTransferPcPane() {
    QListWidgetItem* item = ui->driveList->selectedItems().at(0);
    QString mountpoint;
    QString dev = item->text().mid(item->text().indexOf("(") + 1, item->text().indexOf(")") - item->text().indexOf("(") - 1);
    //qDebug() << "Mounting " + dev;
    if (udisks->blockDevice(dev)->fileSystem()->mountPoints().count() == 0) {
        mountpoint = udisks->blockDevice(dev)->fileSystem()->mount();
    } else {
        mountpoint = udisks->blockDevice(dev)->fileSystem()->mountPoints().at(0);
    }

    if (mountpoint == "") {
        QMessageBox::critical(this, "Couldn't mount", "Couldn't mount " + dev + ".", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    QDir mountDir(mountpoint);
    if (mountDir.cd("theOSTransferData")) {
        ui->TransferList->clear();
        ui->DataNotFoundFrame->setVisible(false);

        for (QString file : mountDir.entryList()) { //Get Directories
            if (file == "." || file == "..") {
                continue;
            }
            if (QDir(mountDir.path() + "/" + file).exists()) {
                QListWidgetItem *item = new QListWidgetItem(file);
                item->setIcon(QIcon::fromTheme("computer"));
                ui->TransferList->addItem(item);
            }
        }
        AllTransfersDir = mountDir;
    } else {
        ui->DataNotFoundFrame->setVisible(true);
    }
    ui->forwardButton->setEnabled(false);
}

void HardDiskDialog::SetupTransferPane() {
    ui->forwardButton->setEnabled(false);
    ui->backButton->setEnabled(false);
    ui->CountingFrame->setVisible(true);

    root = QDir(AllTransfersDir.path());
    root.cd(ui->TransferList->selectedItems().at(0)->text());

    QThread *thread = new QThread;
    CountWorker *worker = new CountWorker(root);
    worker->moveToThread(thread);
    connect(thread, SIGNAL (started()), worker, SLOT (startWork()));
    connect(worker, SIGNAL (finished(qulonglong, QTreeWidgetItem*)), thread, SLOT (quit()));
    connect(worker, SIGNAL (finished(qulonglong, QTreeWidgetItem*)), worker, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    connect(worker, SIGNAL (finished(qulonglong, QTreeWidgetItem*)), this, SLOT (countWorkerDone(qulonglong, QTreeWidgetItem*)));
    thread->start();
}

void HardDiskDialog::countWorkerDone(qulonglong size, QTreeWidgetItem* rootItem) {
    ui->bytesTakenLabel->setText("This transfer will take up " + calculateSize(size) + " of space.");
    ui->CountingFrame->setVisible(false);
    ui->itemsToTransfer->clear();

    transferSize = size;

    for (QTreeWidgetItem* child : rootItem->takeChildren()) {
        ui->itemsToTransfer->addTopLevelItem(child);
        userMapping.insert(child->text(0), "");
    }

    ui->forwardButton->setEnabled(true);
    ui->backButton->setEnabled(true);
}

void HardDiskDialog::SetupDrivePane() {
    ui->driveList->clear();

    QProcess *lsblk = new QProcess(this);
    lsblk->start("lsblk -rf --output name,label,hotplug,parttype");

    lsblk->waitForFinished();
    QByteArray output = lsblk->readAllStandardOutput();

    for (QString block : udisks->blockDevices()) {
        UDisks2Block *device = udisks->blockDevice(block);
        QListWidgetItem *item;
        QIcon icon;
        if (device->fileSystem()) {
            if (device->type != "swap") {
                for (QString part : QString(output).split("\n")) {
                    if (part != "") {
                        QStringList parse = part.split(" ");
                        if (parse.length() > 1) {
                            if (parse[0] == device->fileSystem()->name) {

                                if (parse[1] == "") {
                                    item = new QListWidgetItem(calculateSize(device->size) + " Hard Drive (" + device->fileSystem()->name + ")");
                                    icon = QIcon::fromTheme("drive-harddisk");
                                } else {
                                    if (parse.count() > 2) {
                                        if (parse[2] == "0") {
                                            icon = QIcon::fromTheme("drive-harddisk");
                                        } else {
                                            icon = QIcon::fromTheme("drive-removable-media");
                                        }
                                    }
                                    QString itemText(parse[1].replace("\\x20", " ") + " (" + device->fileSystem()->name + ")");
                                    item = new QListWidgetItem(itemText);
                                }
                            }
                        }
                    }
                }

                if (!(item)) {
                    item = new QListWidgetItem(calculateSize(device->size) + " Hard Drive (" + device->fileSystem()->name + ")");
                    icon = QIcon::fromTheme("drive-harddisk");
                }

                item->setIcon(icon);
                ui->driveList->addItem(item);

            }
        }
    }
}

void HardDiskDialog::on_forwardButton_clicked()
{
    if (currentPane == Environment) {
        QFile *adapter = new QFile("/sys/class/power_supply/AC/online");
        if (!adapter->exists()) {
            adapter = new QFile("/sys/class/power_supply/ADP1/online");
        }
        adapter->open(QFile::ReadOnly);
        QByteArray info = adapter->read(1);
        adapter->close();
        if (QString(info).startsWith("0")) {
            if (QMessageBox::warning(this, "Continue?", "If power is lost during the transfer, items may not transfer properly. Are you sure you wish to continue?", QMessageBox::No, QMessageBox::Yes) == QMessageBox::No) {
                return;
            }
        }
    }
    switchToPane(paneTypes(currentPane + 1));
}

void HardDiskDialog::on_backButton_clicked()
{
    switchToPane(paneTypes(currentPane - 1));
}

void HardDiskDialog::on_itemsToTransfer_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{

}

void HardDiskDialog::on_driveList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == NULL) {
        ui->forwardButton->setEnabled(false);
    } else {
        ui->forwardButton->setEnabled(true);
    }
}

void HardDiskDialog::on_TransferList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == NULL) {
        ui->forwardButton->setEnabled(false);
    } else {
        ui->forwardButton->setEnabled(true);
    }
}

void HardDiskDialog::on_backButton_2_clicked()
{
    NewUser* u = new NewUser(this);
    u->exec();
    delete u;
    SetupUserSetupPane();
}
