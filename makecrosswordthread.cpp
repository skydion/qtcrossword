#include "makecrosswordthread.h"

makeCrosswordThread::makeCrosswordThread(QList<wordInfo*> &lwi, QObject *parent) :
  QThread(parent), wi(lwi)
{
  vocabularyId = 0;
  count = 0;
}

void makeCrosswordThread::run(void)
{
  if (stop)
    return;

  if (vocabularyId == 0)
    return;

  // перше слово в шаблоні
  int curWord = 0;

  // кільки слів ми знайшли
  int foundedWords = 0;

  //
  // Знаходимо перше слово якщо стек пустий. А що коли не пустий? Чому виходимо?
  //
  if ( findWord(curWord) && workStack.isEmpty())
    {
      workStack.push(curWord);
      foundedWords++;
    }
  else
    {
      qDebug() << tr("makeCrossword: Can't find any word for startup!");
      emit showStatusBarMessage( tr("makeCrossword: Can't find any word for startup!") );
      return;
    }

  /* зробити ще одну змінну яка буде враховувати кількість знайдених слів і якщо вона не пуста
   * треба шукати слова з незаповненими перетинами і для них шукати відповідні до шаблону слова
   */
  while ( !workStack.isEmpty() )
    {
      int curCross = 0;
      while (curCross < wi[curWord]->crossCount)
        {
          int crossedWord = wi[curWord]->cil[curCross]->numWord;

          if ( !wi[crossedWord]->filled )
            {
              do
                {
                  if ( findWord(crossedWord) )
                    {
                      workStack.push(crossedWord);
                      curCross = 0;
                      foundedWords++;
                      curWord = crossedWord;
                      break;
                    }
                  else
                    {
                      clearWord(crossedWord);

                      if ( workStack.isEmpty() )
                        {
                          qDebug() << tr("makeCrossword: Stack is empty!");
                          emit showStatusBarMessage( tr("makeCrossword: Stack is empty!") );
                          return;
                        }
                      else
                        {
                          crossedWord = workStack.pop();
                          foundedWords--;
                        }
                    }
                }
              while ( true );
            }
          else
            {
              if ( wi[curWord]->crossCount == 1 )
                workStack.push(crossedWord);

              foundedWords++;
              curCross++;
            }
        }

      qDebug() << "run: foundedWords[" << foundedWords << "], " << "countWords[" << wi.count() << "]";

      // Подивитися що робить ця умова. Як я пригадую, це може бути на фінішну перевірку стеку
      // тобто десь можуть бути окремі блоки не привязані до основного кросворду, ніби
      // кросворд в кросворді. Або якесь інше призначення, може і лишня умова.
      if ( workStack.isEmpty() )
        {
          qDebug() << tr("makeCrossword: Done");
          emit showStatusBarMessage( tr("makeCrossword: Done") );
          return;
        }
      else
        {
          curWord = workStack.pop();
          foundedWords--;
        }
    }
}

bool makeCrosswordThread::findWord(int curWord)
{
  QString curWordTemplate = createTemplate(curWord);

  QRegExp rx(curWordTemplate);

  bool match = false;
  int li = wi[curWord]->listIndex;

  for (int i = li; i < count; i++)
    {
      if (wd[i]->used == false)
        {
          match = rx.exactMatch(wd[i]->word);

          if (match)
            {
              wi[curWord]->listIndex = wd[i]->id;
              wi[curWord]->wordId = wd[i]->id;
              wi[curWord]->text = wd[i]->word;
              wi[curWord]->filled = true;

              wd[i]->used = true;

              return true;
            }
        }
    }

  //    int id = -1, countRecord = -1, vocabulary = 29;
  //    QString result;
  //    QSqlQuery query;
  //    QSqlError le;
  //
  //    QString curWordTemplate = createTemplate(curWord);
  //
  //    query.prepare("SELECT _id, _word FROM slovopedia.works WHERE lower(_word) "
  //		  "LIKE ? AND _id > ? AND _vocabulary = ? AND _used = 0 LIMIT 1;");
  //
  //    query.addBindValue(QVariant(curWordTemplate));
  //    query.addBindValue(QVariant(wi[curWord]->wordId));
  //    query.addBindValue(QVariant(vocabulary));
  //    query.exec();
  //
  //    le = query.lastError();
  //    if (le.type() == QSqlError::NoError)
  //    {
  //	if (query.isActive() && query.isSelect())
  //	    countRecord = query.size();
  //	else
  //	    countRecord = -1;
  //
  //	if (countRecord > 0)
  //	{
  //	    int idNo = query.record().indexOf("_id");
  //	    int wordNo = query.record().indexOf("_word");
  //
  //	    while (query.next())
  //	    {
  //		id = query.value(idNo).toInt();
  //		result = query.value(wordNo).toString();
  //
  //		wi[curWord]->wordId = id;
  //		wi[curWord]->text = result;
  //		wi[curWord]->filled = true;
  //	    }
  //
  //	    query.finish();
  //
  //	    QSqlQuery query;
  //	    query.prepare("UPDATE slovopedia.works SET _used = 1 WHERE _id = ?;");
  //	    query.addBindValue(QVariant(id));
  //	    query.exec();
  //
  //	    return true;
  //	}
  //    }
  //

  return false;
}

QString makeCrosswordThread::createTemplate(int curWord)
{
  wordInfo *w = wi[curWord], *w2 = NULL;
  crossInfo *c = NULL, *c2 = NULL;

  QString strRegExp(w->length, QChar('.'));

  for (int i = 0; i < w->crossCount; i++)
    {
      c = w->cil[i];

      int nw2 = c->numWord;
      w2 = wi[nw2];

      if (w2->filled)
        {
          for (int j = 0; j < w2->crossCount; j++ )
            {
              c2 = w2->cil[j];

              if (curWord == c2->numWord)
                strRegExp[c->crossPos] = w2->text[c2->crossPos];
            }
        }
    }

  return strRegExp.toLower();
}

void makeCrosswordThread::clearWord(int crossedWord)
{
  wordInfo *w = wi[crossedWord];

  w->text = "";
  w->filled = false;

  for (int i = 0; i < count; i++)
    {
      if (w->wordId == wd[i]->id)
        wd[i]->used = false;
    }

  //    QSqlQuery query;
  //    query.prepare("UPDATE slovopedia.works SET _used = 0 WHERE _id = ?;");
  //    query.addBindValue(QVariant(w->wordId));
  //    query.exec();
}

void makeCrosswordThread::startScanning(void)
{
  stop = false;
}

void makeCrosswordThread::stopScanning(void)
{
  stop = true;
}

void makeCrosswordThread::setData(QList<wordInfo*> &lwi)
{
  wi = lwi;
}

void makeCrosswordThread::setVocabulary(int vocabulary)
{
  if (vocabularyId == vocabulary)
    return;

  vocabularyId = vocabulary;

  wd.clear();
  count = 0;

  int id = -1, countRecord = -1;

  QString word;
  QSqlQuery query;
  QSqlError le;
  wordData *WD = NULL;

  query.prepare("SELECT _id, _word FROM slovopedia.works WHERE _vocabulary = ?;");
  query.addBindValue(QVariant(vocabulary));
  query.exec();

  le = query.lastError();
  if (le.type() == QSqlError::NoError)
    {
      if (query.isActive() && query.isSelect())
        countRecord = query.size();
      else
        countRecord = -1;

      if (countRecord > 0)
        {
          int idNo = query.record().indexOf("_id");
          int wordNo = query.record().indexOf("_word");

          while (query.next())
            {
              id = query.value(idNo).toInt();
              word = query.value(wordNo).toString().toLower();

              int idx = word.indexOf(QRegExp("[,-\\s]"), 0);

              if (idx == -1)
                {
                  WD = new wordData();

                  if (WD)
                    {
                      WD->id = id;
                      WD->word = word;

                      wd.append(WD);
                    }
                }
            }
        }
    }

  count = wd.count();
}
