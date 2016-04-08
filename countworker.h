#ifndef COUNTWORKER_H
#define COUNTWORKER_H

#include <QObject>
#include <QThread>
#include <QDir>
#include <QDirIterator>
#include <QTreeWidgetItem>

class CountWorker : public QObject
{
    Q_OBJECT
public:
    explicit CountWorker(QDir directory, QObject *parent = 0);

signals:
    void finished(qulonglong size, QTreeWidgetItem* rootItem);

public slots:
    void startWork();

private:
    QDir selectedDirectory;
};

#endif // COUNTWORKER_H
