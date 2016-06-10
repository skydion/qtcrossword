#ifndef TABLECROSSWORDGRIDWIDGET_H
#define TABLECROSSWORDGRIDWIDGET_H

#include <QtSql>
#include <QSqlDatabase>
#include <QList>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetSelectionRange>
#include <QMouseEvent>
#include <QPoint>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QProgressBar>
#include <QStatusBar>
#include <QStack>
#include <QPrinter>
#include <QPaintDevice>
#include <QMessageBox>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "ui_SizeOfTemplate.h"

#include "crosswordinfo.h"
#include "makecrosswordthread.h"

class tableTemplateWidget : public QTableWidget
{
  Q_OBJECT
public:
  explicit tableTemplateWidget(QWidget *parent = 0);
  ~tableTemplateWidget();

  void setTemplateId(int id) { templateId = id; }
  void setStatusBar(QStatusBar *qsb);
  bool checkDirty(void) { return isDirty; }
  void setDB(QSqlDatabase *sqlDB) { db = sqlDB; }
  QTextDocument* getDocument(void) { return &doc; }

protected:
  QMenu *menu;
  QAction *actionRmColumn, *actionRmRow, *actionInsColumn, *actionInsRow;
  QList<QTableWidgetSelectionRange> selectionRange;

  bool symetricalMode;

  int templateId;

  // кількість рядків та стовпців в шаблоні
  int numRow, numCol;

  // кількість слів в шаблоні
  int countWords;

  bool isDirty;
  bool isMaked;

  enum orientation { horizontal = false, vertical = true };
  int numWord;
  int DRC[4][2];
  int Crosses[9];

  QList<wordInfo*> wi;

  QColor fullCell, emptyCell;
  QFont font;
  QProgressBar *pb;
  QStatusBar *sb;

  QSqlDatabase *db;

  Ui::SizeOfTemplate *sizeOfTemplate;
  makeCrosswordThread *mct;
  QTextDocument doc;

  QString questionsH, questionsV;

protected:
  void contextMenuEvent(QContextMenuEvent *e);
  void updateCell(QTableWidgetItem *cell, int value);
  void newCell(int row, int col, int value);

  void scanTemplate(void);
  void scanHorizontal(void);
  void scanVertical(void);
  void saveResult(int row, int col,  int lenght, int orient, int crossCount);
  int isCrosses(int row, int col);
  int findCrossedWord(int row, int col, int nw);
  void displayCrossword(void);
  void savePrivateData(void);
  void loadPrivateData(void);
  void prepareQuestions(void);

signals:
  // need for templateListWidget
  void savedToDB(int id);
  void deletedFromDB(int id);
  void maked(void);

public slots:
  void loadFromDB(void);
  void saveToDB(void);
  void createNew(void);
  void deleteTemplate(void);
  void setSymetricalMode(bool value) { symetricalMode = value; }
  void makeCrossword(void);
  void printPreview(QPrinter *prn);
  void setVocabulary(int vocabularyId);

protected slots:
  void rmColumn(void);
  void rmRow(void);
  void insColumn(void);
  void insRow(void);

  void setClickedCell(int row, int column);
  void makeFinished(void);
};

#endif // TABLECROSSWORDGRIDWIDGET_H
