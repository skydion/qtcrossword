#ifndef MAKECROSSWORDTHREAD_H
#define MAKECROSSWORDTHREAD_H

#include <QDebug>
#include <QThread>
#include <QString>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QStack>
#include <QList>
#include <QVariant>

#include "crosswordinfo.h"

class wordData
{
public:
    int id;
    QString word;
    QString question;
    bool used;

public:
    wordData()
    {
	id = 0;
	used = false;
    }
};

class makeCrosswordThread : public QThread
{
Q_OBJECT
public:
    explicit makeCrosswordThread(QList<wordInfo*> &lwi, QObject *parent = 0);

    void startScanning(void);
    void stopScanning(void);
    void setData(QList<wordInfo*> &lwi);
    void setVocabulary(int vocabulary);

protected:
    void run(void);

    bool findWord(int curWord);
    QString createTemplate(int curWord);
    void clearWord(int crossedWord);

signals:

public slots:

private:
    bool stop;
    QList<wordInfo*> &wi;
    QStack<int> workStack;

    int vocabularyId;
    int count;
    QList<wordData*> wd;
};

#endif // MAKECROSSWORDTHREAD_H
