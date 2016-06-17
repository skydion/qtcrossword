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

/*
 * слова зі словнику
 */
class wordData
{
public:
  //
  int id;

  // слово зі словника
  QString word;

  // запитання для слова
  QString question;

  // вже використане слово чи можна використати
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
  void showStatusBarMessage(QString msg);

public slots:

private:
  bool stop;

  QList<wordInfo*> &wi;

  /* список слів завантажений з БД
   * для швидкодії? може потім викинути
   */
  QList<wordData*> wd;

  QStack<int> workStack;

  // словник по якому складаємо кросворд
  int vocabularyId;

  // кількість слів в словнику по якому складаємо кросворд
  int numberOfWordsInVocabulary;
};

#endif // MAKECROSSWORDTHREAD_H
