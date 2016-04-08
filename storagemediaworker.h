#ifndef STORAGEMEDIAWORKER_H
#define STORAGEMEDIAWORKER_H

#include <QObject>
#include <QDir>
#include <QMap>
#include <QDebug>
#include <QDirIterator>
#include <QProcess>
#include <QApplication>

class StorageMediaWorker : public QObject
{
    Q_OBJECT
public:
    explicit StorageMediaWorker(QDir rootDir, QMap<QString, QString> userMapping, qulonglong size, QObject *parent = 0);

signals:
    void finished();
    void progressChanged(qulonglong value, qulonglong max);

public slots:
    void startWork();

private:
    QDir rootDir;
    QMap<QString, QString> userMapping;
    qulonglong size;
};

#endif // STORAGEMEDIAWORKER_H
