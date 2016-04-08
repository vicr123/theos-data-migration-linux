#include "countworker.h"

CountWorker::CountWorker(QDir directory, QObject *parent) : QObject(parent)
{
    this->selectedDirectory = directory;
}

void CountWorker::startWork() {
    qulonglong totalBytes = 0;
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();

    //for (QString file : this->selectedDirectory.path()) {
        //if (QDir(file).exists()) {
            QDirIterator *iterator = new QDirIterator(this->selectedDirectory, QDirIterator::Subdirectories);
            QTreeWidgetItem* currentUserItem;

            while (iterator->hasNext()) {
                QString processFile = iterator->next();
                if (QDir(processFile).exists()) {
                    QString dirRelative = processFile.remove(0, this->selectedDirectory.path().length());
                    QStringList directoryPath = dirRelative.split("/");
                    directoryPath.removeFirst();
                    if (!(directoryPath.contains(".") || directoryPath.contains(".."))) {
                        if (directoryPath.count() == 1) {
                            currentUserItem = new QTreeWidgetItem();
                            if (directoryPath.at(0) == "generic") {
                                currentUserItem->setText(0, "Other Folders");
                                currentUserItem->setIcon(0, QIcon::fromTheme("folder"));
                            } else {
                                currentUserItem->setText(0, directoryPath.at(0));
                                currentUserItem->setIcon(0, QIcon::fromTheme("user"));
                            }
                            rootItem->addChild(currentUserItem);
                        } else if (directoryPath.count() == 2) {
                            QTreeWidgetItem* currentItem = new QTreeWidgetItem();
                            currentItem->setText(0, directoryPath.at(1));
                            if (directoryPath.at(1) == "Documents") {
                                currentItem->setIcon(0, QIcon::fromTheme("folder-documents"));
                            } else if (directoryPath.at(1) == "Music") {
                                currentItem->setIcon(0, QIcon::fromTheme("folder-music"));
                            } else if (directoryPath.at(1) == "Videos") {
                                currentItem->setIcon(0, QIcon::fromTheme("folder-video"));
                            } else if (directoryPath.at(1) == "Pictures") {
                                currentItem->setIcon(0, QIcon::fromTheme("folder-images"));
                            } else {
                                currentItem->setIcon(0, QIcon::fromTheme("folder"));
                            }
                            currentUserItem->addChild(currentItem);
                        }
                    }
                } else {
                    /*QString pRoot = processFile.left(processFile.lastIndexOf("/"));
                    pRoot.remove(file);
                    pRoot.remove(0, 1);
                    if (pRoot == "") {
                        pRoot = "";
                    } else if (pRoot == "." || pRoot == "..") {
                        continue;
                    }*/
                    totalBytes += QFileInfo(processFile).size();
                }
            //}
        //}
    }

    emit finished(totalBytes, rootItem);
}
