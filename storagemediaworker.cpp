#include "storagemediaworker.h"

StorageMediaWorker::StorageMediaWorker(QDir rootDir, QMap<QString, QString> userMapping, qulonglong size, QObject *parent) : QObject(parent)
{
    this->rootDir = rootDir;
    this->userMapping = userMapping;
    this->size = size;
}

void StorageMediaWorker::startWork() {
    qulonglong read = 0;
    QStringList users = rootDir.entryList();
    for (QString user : users) {
        QDir userPath(rootDir.path() + "/" + user);
        if (userPath.exists()) {
            if (user == "generic") {
                user = "Other Folders";
            }
            QString systemUser = userMapping.value(user);
            if (systemUser == "") {
                qDebug() << user + " not found!";
            } else if (systemUser == "nobody") {

            } else {
                QDir systemUserHome("/home/" + systemUser);

                QStringList root;
                QStringList filesToCopy;


                QDirIterator *iterator = new QDirIterator(userPath, QDirIterator::Subdirectories);

                while (iterator->hasNext()) {
                    QString processFile = iterator->next();
                    if (QDir(processFile).exists()) {
                        processFile.remove(userPath.path() + "/");
                        systemUserHome.mkdir(processFile);
                        QProcess::startDetached("chown " + systemUser + " \"" + systemUserHome.path() + "/" + processFile + "\"");
                    } else {
                        QString pRoot = processFile.left(processFile.lastIndexOf("/"));
                        pRoot.remove(userPath.path());
                        pRoot.remove(0, 1);
                        if (pRoot == "") {
                            pRoot = "";
                        } else if (pRoot == "." || pRoot == "..") {
                            continue;
                        }
                        root.append(pRoot);
                        filesToCopy.append(processFile);
                    }
                }

                int i = 0;
                for (QString file : filesToCopy) {
                    QString destination = systemUserHome.path();
                    destination.append("/" + root.at(i));
                    QFile src(file);
                    QFileInfo info(src);
                    QFile d(destination.append("/" + info.fileName()));
                    if (info.fileName() == "." || info.fileName() == "..") {
                        continue;
                    }

                    src.open(QFile::ReadOnly);
                    d.open(QFile::WriteOnly);

                    while (src.bytesAvailable() > 0) {
                        QByteArray buf = src.read(4194304);
                        d.write(buf);
                        read += buf.length();
                        emit progressChanged(((float) read / (float) this->size) * 100, 100);
                    }

                    src.close();
                    d.close();
                    QProcess::startDetached("chown " + systemUser + " \"" + d.fileName() + "\"");
                    i++;
                }
            }
        }
    }

    emit progressChanged(0, 0);
    emit finished();
}
