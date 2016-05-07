#ifndef CROSSWORDINFO_H
#define CROSSWORDINFO_H

#include <QList>

class crossInfo
{
public:
    int crossPos; // потрібно на етапі сканування і складання
    int crossType; // потрібно на етапі сканування і складання
    int numWord; // потрібно на етапі сканування і складання

    int numWord2; // потрібно на етапі сканування
    int row; // потрібно на етапі сканування
    int col; // потрібно на етапі сканування
    int crossIndex; // потрібно на етапі сканування

public:
    crossInfo()
    {
	numWord = numWord2 = 0;
	row = col = 0;
	crossIndex = crossPos = crossType = 0;
    }
};

class wordInfo
{
public:
    int numWord;
    bool filled;
    int row;
    int col;
    int length;
    bool orient;
    int crossCount;
    int wordId;
    int listIndex; // для тестування пошуку по списку в пам"яті при роботі з БД не порібно
    QString text;

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
