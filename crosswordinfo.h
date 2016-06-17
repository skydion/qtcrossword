#ifndef CROSSWORDINFO_H
#define CROSSWORDINFO_H

#include <QList>

class crossInfo
{
public:
  // порядкова позиція перетину в слові
  int crossPos;

  // тип перетину
  int crossType;

  // порядковий номер слова в шаблоні
  int currentWord;

  // порядковий номер іншого слова з яким утворється даний перетин в цьому слові
  int intersectedWord;

  // рядок перетину в шаблоні, щоб знати чи є перетини з інших слів в цьому рядку
  int row;

  // стовпчик перетину в шаблоні, щоб знати чи є перетини з інших слів в цьому стовпчику
  int col;

public:
  crossInfo()
  {
    currentWord = intersectedWord = 0;
    row = col = 0;
    crossPos = crossType = 0;
  }
};

class wordInfo
{
public:
  // порядковий номер слова в шаблоні
  int numWord;

  // слово заповнено чи ні
  bool filled;

  // рядок початку слова в шаблоні
  int row;

  // стовпчик початку слова в шаблоні
  int col;

  // довжина слова
  int length;

  // орієнтація слова (горизонтальна, вертикальна)
  bool orient;

  // кількість перетинів в слові
  int crossCount;

  // ?
  int wordId;

  // для тестування пошуку по списку в пам"яті, при роботі з БД не потрібно
  // позиція слова в загальному словнику
  int listIndex;

  // знайдене слово
  QString text;

  /*
   * список з інформаціює про перетини слова з іншими словами,
   * дозволяє рухатися по кросворду вперід-назад
   */
  QList<crossInfo*> cil;

public:
  wordInfo()
  {
    numWord = wordId = listIndex = 0;
    filled = orient = false;
    row = col = 0;
    length = crossCount = 0;
  }
};

#endif // CROSSWORDINFO_H
