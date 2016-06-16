#include "tabletemplatewidget.h"

tableTemplateWidget::tableTemplateWidget(QWidget *parent) :
  QTableWidget(parent)
{
  templateId = 0;
  numRow = numCol = 0;

  countWords = 0;

  isDirty = false;
  isMaked = false;

  sizeOfTemplate = NULL;

  DRC[0][0] = -1;
  DRC[0][1] = 0;

  DRC[1][0] = 0;
  DRC[1][1] = 1;

  DRC[2][0] = 1;
  DRC[2][1] = 0;

  DRC[3][0] = 0;
  DRC[3][1] = -1;

  intersections[0] = 3;
  intersections[1] = 6;
  intersections[2] = 7;
  intersections[3] = 9;
  intersections[4] = 11;
  intersections[5] = 12;
  intersections[6] = 13;
  intersections[7] = 14;
  intersections[8] = 15;

  fullCell = Qt::yellow;
  emptyCell = Qt::darkRed;

  font.setBold(true);
  font.setFamily("Tahoma");
  font.setPointSize(14);

  actionRmColumn = new QAction("Remove column", this);
  connect(actionRmColumn, SIGNAL(triggered()), this, SLOT(rmColumn()));

  actionInsColumn = new QAction("Insert column", this);
  connect(actionInsColumn, SIGNAL(triggered()), this, SLOT(insColumn()));

  actionRmRow = new QAction("Remove row", this);
  connect(actionRmRow, SIGNAL(triggered()), this, SLOT(rmRow()));

  actionInsRow = new QAction("Insert row", this);
  connect(actionInsRow, SIGNAL(triggered()), this, SLOT(insRow()));

  menu = new QMenu(this);

  if (menu)
    {
      menu->addAction(actionRmColumn);
      menu->addAction(actionInsColumn);
      menu->addSeparator();
      menu->addAction(actionRmRow);
      menu->addAction(actionInsRow);
    }

  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setSelectionMode(QAbstractItemView::ContiguousSelection);

  this->horizontalHeader()->setDefaultSectionSize(40);
  this->verticalHeader()->setDefaultSectionSize(40);

  connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(setClickedCell(int, int)));

  mct = new makeCrosswordThread(wi, this);

  QTextOption to;
  to.setWrapMode(QTextOption::WordWrap);
  to.setAlignment(Qt::AlignJustify);

  printDoc.setDefaultTextOption(to);
  //doc.setDefaultFont(font);
}

tableTemplateWidget::~tableTemplateWidget()
{
  if (menu)
    delete menu;

  if (actionRmColumn)
    delete actionRmColumn;

  if (actionRmRow)
    delete actionRmRow;

  if (actionInsColumn)
    delete actionInsColumn;

  if (actionInsRow)
    delete actionInsRow;

  if (mct)
    {
      mct->stopScanning();

      if (mct->wait())
        delete mct;
    }
}

void tableTemplateWidget::contextMenuEvent(QContextMenuEvent *e)
{
  QList<QTableWidgetSelectionRange> listRanges = selectedRanges();

  if (listRanges[0].columnCount() == 1)
    {
      actionRmRow->setDisabled(true);
      actionInsRow->setDisabled(true);
    }

  if (listRanges[0].rowCount() == 1)
    {
      actionRmColumn->setDisabled(true);
      actionInsColumn->setDisabled(true);
    }

  menu->exec(e->globalPos());

  actionRmRow->setDisabled(false);
  actionInsRow->setDisabled(false);
  actionRmColumn->setDisabled(false);
  actionInsColumn->setDisabled(false);
}

void tableTemplateWidget::rmColumn(void)
{
  int lc, cc;

  selectionRange = selectedRanges();

  for (int i = 0; i < selectionRange.count(); i++)
    {
      lc = selectionRange[i].leftColumn();
      cc = selectionRange[i].columnCount();

      for (int j = 0; j < cc; j++)
        removeColumn(lc);
    }

  numCol = columnCount();
  isDirty = true;
}

void tableTemplateWidget::rmRow(void)
{
  int tr, rc;

  selectionRange = selectedRanges();

  for (int i = 0; i < selectionRange.count(); i++)
    {
      tr = selectionRange[i].topRow();
      rc = selectionRange[i].rowCount();

      for (int j = 0; j < rc; j++)
        removeRow(tr);
    }

  numRow = rowCount();
  isDirty = true;
}

void tableTemplateWidget::insColumn(void)
{
  int lc, cc;
  int i, j;

  selectionRange = selectedRanges();

  for (i = 0; i < selectionRange.count(); i++)
    {
      lc = selectionRange[i].leftColumn();
      cc = selectionRange[i].columnCount();

      for (j = 0; j < cc; j++)
        insertColumn(lc);
    }

  for (i = 0; i < numRow; i++)
    {
      for (j = 0; j < cc; j++)
        newCell(i, lc+j, 0);
    }

  numCol = columnCount();
  isDirty = true;
}

void tableTemplateWidget::insRow(void)
{
  int tr, rc;
  int i, j;

  selectionRange = selectedRanges();

  for (i = 0; i < selectionRange.count(); i++)
    {
      tr = selectionRange[i].topRow();
      rc = selectionRange[i].rowCount();

      for (j = 0; j < rc; j++)
        insertRow(tr);
    }

  for (i = 0; i < rc; i++)
    {
      for (j = 0; j < numCol; j++)
        newCell(tr+i, j, 0);
    }

  numRow = rowCount();
  isDirty = true;
}

/*
 * Завантажуємо шаблон кросворду з БД
 */
void tableTemplateWidget::loadFromDB()
{
  if (templateId == 0)
    return;

  isMaked = false;

  int countRecord = -1;
  int rowNo, colNo, valNo, countWordsNo;
  int row, col, val;
  QSqlQuery query;

  query.prepare("SELECT _rows, _columns, _count_words FROM crossword.templates WHERE _id = ?;");
  query.addBindValue(QVariant(templateId));
  query.exec();

  QSqlError le = query.lastError();
  if (le.type() == QSqlError::NoError)
    {
      if (query.isActive() && query.isSelect())
        countRecord = query.size();
      else
        countRecord = -1;

      if (countRecord > 0)
        {
          query.first();

          rowNo = query.record().indexOf("_rows");
          colNo = query.record().indexOf("_columns");
          countWordsNo = query.record().indexOf("_count_words");

          numRow = query.value(rowNo).toInt();
          numCol = query.value(colNo).toInt();
          countWords = query.value(countWordsNo).toInt();

          setRowCount(numRow);
          setColumnCount(numCol);
        }
      query.finish();
    }
  else
    qDebug() << "loadFromDB: " << le.text();

  query.prepare("SELECT _row, _column, _value FROM crossword.grids WHERE _template = ?;");
  query.addBindValue(QVariant(templateId));
  query.exec();

  le = query.lastError();
  if (le.type() == QSqlError::NoError)
    {
      if (query.isActive() && query.isSelect())
        countRecord = query.size();
      else
        countRecord = -1;

      if (countRecord > 0 && countRecord == numRow*numCol)
        {
          rowNo = query.record().indexOf("_row");
          colNo = query.record().indexOf("_column");
          valNo = query.record().indexOf("_value");

          while (query.next())
            {
              row = query.value(rowNo).toInt();
              col = query.value(colNo).toInt();
              val = query.value(valNo).toInt();

              newCell(row, col, val);
            }
          query.finish();
        }
    }
  else
    qDebug() << "loadFromDB: " << le.text();

  loadPrivateData();
}

/*
 * Завантажуємо приватні дані по шаблону решітки для подальшого пошуку слів
 */
void tableTemplateWidget::loadPrivateData(void)
{
  wi.clear();

  QSqlQuery query, query2;
  QSqlError le;
  QSqlRecord rec;

  int countRecord = -1;
  int idNo = 0, numwordNo = 0, rowNo = 0, columnNo = 0;
  int lenghtNo = 0, crosscountNo = 0, orientationNo = 0;

  wordInfo *w = NULL;
  crossInfo *c = NULL;

  query.prepare("SELECT _id, _numword, _row, _column, _lenght, _crosscount, "
                "_orientation FROM crossword.private_data "
                "WHERE _template = ? ORDER BY _numword;");
  query.addBindValue(QVariant(templateId));
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
          rec = query.record();

          idNo = rec.indexOf("_id");
          numwordNo = rec.indexOf("_numword");
          rowNo = rec.indexOf("_row");
          columnNo = rec.indexOf("_column");
          lenghtNo = rec.indexOf("_lenght");
          crosscountNo = rec.indexOf("_crosscount");
          orientationNo = rec.indexOf("_orientation");

          while (query.next())
            {
              w = new wordInfo();
              if (w)
                {
                  w->numWord = query.value(numwordNo).toInt();
                  w->row = query.value(rowNo).toInt();
                  w->col = query.value(columnNo).toInt();
                  w->length = query.value(lenghtNo).toInt();
                  w->orient = query.value(orientationNo).toBool();
                  w->crossCount = query.value(crosscountNo).toInt();

                  query2.prepare("SELECT _cpos, _ctype, _numword "
                                 "FROM crossword.crosses WHERE _pd_id = ?;");
                  query2.addBindValue(query.value(idNo));
                  query2.exec();

                  le = query2.lastError();
                  if (le.type() == QSqlError::NoError)
                    {
                      if (query2.isActive() && query2.isSelect())
                        countRecord = query2.size();
                      else
                        countRecord = -1;

                      if (countRecord > 0)
                        {
                          int cposNo = query2.record().indexOf("_cpos");
                          int ctypeNo = query2.record().indexOf("_ctype");
                          int numword2No = query2.record().indexOf("_numword");

                          while (query2.next())
                            {
                              c = new crossInfo();

                              if (c)
                                {
                                  c->crossPos = query2.value(cposNo).toInt();
                                  c->crossType = query2.value(ctypeNo).toInt();
                                  c->numWord = query2.value(numword2No).toInt();

                                  w->cil.append(c);
                                  c = NULL;
                                }
                            }
                        }

                      query2.finish();
                    }
                  else
                    qDebug() << "loadPrivateData: " << le.text();

                  wi.append(w);
                  w = NULL;
                }
            }
        }

      query.finish();
    }
  else
    qDebug() << "loadPrivateData: " << le.text();
}


/*
 * Зберігаємо шаблон кросворду після редагування решітки
 */
void tableTemplateWidget::saveToDB()
{
  int previewSizeCell = 20, val;
  int W = numCol*previewSizeCell;
  int H = numRow*previewSizeCell;

  QPainter *pngPainter = new QPainter();
  QImage *image = new QImage(QSize(W, H), QImage::Format_ARGB32_Premultiplied);

  float t, l;
  QRectF src(0, 0, W, H);
  QRectF r;

  QTableWidgetItem *cell;
  QSqlQuery query;

  query.prepare("DELETE FROM crossword.grids WHERE _template = ?;");
  query.addBindValue(QVariant(templateId));
  query.exec();

  QSqlError le = query.lastError();
  if (le.type() != QSqlError::NoError)
    qDebug() << "saveToDB: " << le.text();

  query.prepare("INSERT INTO crossword.grids (_template, _row, _column, _value) "
                "VALUES (?, ?, ?, ?)");

  pb = new QProgressBar(this);
  pb->setRange(0, numRow*numCol);
  sb->addWidget(pb);

  QVariantList tmp, row, col, value;
  QString text;

  pngPainter->begin(image);
    int i, j, nw;
    for (i = 0; i < numRow; i++)
      {
        t = src.top() + i * previewSizeCell;
        for (j = 0; j < numCol; j++)
          {
            l = src.left() + j * previewSizeCell;

            r.setTop(t);
            r.setLeft(l);

            r.setRight(src.left() + l + previewSizeCell);
            r.setBottom(src.top() + t + previewSizeCell);

            cell = this->item(i, j);
            val = cell->data(Qt::UserRole).toInt();

            if (val)
              pngPainter->fillRect(r, fullCell);
            else
              pngPainter->fillRect(r, emptyCell);

            // прямокутник для ячейки
            pngPainter->drawRect(r);

            // виводимо номер слова
            nw = findWordByRC(i, j) + 1;
            if ( nw >= 1 )
              {
                text = QVariant(nw).toString();
                pngPainter->drawText(r, Qt::AlignLeft | Qt::AlignTop, text);
              }

            tmp << templateId;
            row << i;
            col << j;
            value << val;
          }

        pb->setValue(i*numRow + j*numCol);
      }
  pngPainter->end();

  query.addBindValue(tmp);
  query.addBindValue(row);
  query.addBindValue(col);
  query.addBindValue(value);

  QSqlDriver *drv = db->driver();
  drv->beginTransaction();
    query.execBatch(QSqlQuery::ValuesAsRows);
  drv->commitTransaction();

  le = query.lastError();
  if (le.type() != QSqlError::NoError)
    qDebug() << "saveToDB: " << le.text();

  QByteArray ba;
  QBuffer blob(&ba);
  blob.open(QIODevice::ReadWrite | QIODevice::Unbuffered);
    image->save(&blob, "PNG");
  blob.close();

  /*
   * ====== Run before update DB ======
   */
  scanTemplate();
  savePrivateData();

  query.prepare("UPDATE crossword.templates SET _rows = ?, _columns = ?, "
                "_preview = ?, _count_words = ? WHERE _id = ?;");
    query.addBindValue(QVariant(numRow));
    query.addBindValue(QVariant(numCol));
    query.addBindValue(QVariant(blob.data()));
    query.addBindValue(QVariant(wi.count()));
    query.addBindValue(QVariant(templateId));
  query.exec();

  le = query.lastError();
  if (le.type() != QSqlError::NoError)
    qDebug() << "saveToDB: " << le.text();

  delete image;
  delete pngPainter;

  sb->removeWidget(pb);
  sb->showMessage(tr("Template saved"), 2000);

  isDirty = false;

  // need for templateListWidget
  emit savedToDB(templateId);
}

void tableTemplateWidget::savePrivateData(void)
{
  int lastId = 0, cc = 0;

  crossInfo *cross;
  QSqlQuery query, query2;
  QSqlError le;

  query.prepare("DELETE FROM crossword.private_data WHERE _template = ?;");
  query.addBindValue(QVariant(templateId));
  query.exec();

  le = query.lastError();
  if (le.type() == QSqlError::NoError)
    {
      query.prepare("DELETE FROM crossword.crosses WHERE _template = ?;");
      query.addBindValue(QVariant(templateId));
      query.exec();

      le = query.lastError();
      if (le.type() != QSqlError::NoError)
        qDebug() << "2. savePrivateData: " << le.text();
    }
  else
    qDebug() << "1.  savePrivateData: " << le.text();

  QVariantList tmp;
  QVariantList pdid;
  QVariantList cpos;
  QVariantList ctype;
  QVariantList nw;

  QSqlDriver *drv = db->driver();

  drv->beginTransaction();
  for (int i = 0; i < wi.count(); i++)
    {
      query.prepare("INSERT INTO crossword.private_data (_template, _numword, "
                    "_row, _column, _lenght, _crosscount, _orientation) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?);");

      query.addBindValue(QVariant(templateId));
      query.addBindValue(QVariant(wi[i]->numWord));
      query.addBindValue(QVariant(wi[i]->row));
      query.addBindValue(QVariant(wi[i]->col));
      query.addBindValue(QVariant(wi[i]->length));
      query.addBindValue(QVariant(wi[i]->crossCount));
      query.addBindValue(QVariant(wi[i]->orient));
      query.exec();

      le = query.lastError();
      if (le.type() == QSqlError::NoError)
        {
          lastId = query.lastInsertId().toInt();

          query2.prepare("INSERT INTO crossword.crosses (_template, _pd_id, _cpos, "
                         "_ctype, _numword) VALUES (?, ?, ?, ?, ?);");

          cc = wi[i]->crossCount;
          for (int j = 0; j < cc; j++)
            {
              cross = wi[i]->cil[j];

              tmp << templateId;
              pdid << lastId;
              cpos << cross->crossPos;
              ctype << cross->crossType;
              nw << cross->numWord2;
            }

          query2.addBindValue(tmp);
          query2.addBindValue(pdid);
          query2.addBindValue(cpos);
          query2.addBindValue(ctype);
          query2.addBindValue(nw);

          query2.execBatch(QSqlQuery::ValuesAsRows);

          tmp.clear();
          pdid.clear();
          cpos.clear();
          ctype.clear();
          nw.clear();
        }
      else
        qDebug() << "3. savePrivateData: " << le.text();
    }
  drv->commitTransaction();
}

/*
 * Створюємо новий пустий шаблон кросворду
 */
void tableTemplateWidget::createNew(void)
{
  QDialog *dialog = new QDialog();
  sizeOfTemplate = new Ui::SizeOfTemplate();
  sizeOfTemplate->setupUi(dialog);

  dialog->setModal(true);
  dialog->exec();

  if (dialog->result() == QDialog::Accepted)
    {
      numRow = sizeOfTemplate->spinRows->value();
      numCol = sizeOfTemplate->spinColumns->value();

      QSqlQuery query;
      query.prepare("INSERT INTO crossword.templates (_rows, _columns) VALUES (?, ?);");
      query.addBindValue(QVariant(numRow));
      query.addBindValue(QVariant(numCol));
      query.exec();

      QSqlError le = query.lastError();
      if (le.type() == QSqlError::NoError)
        templateId = query.lastInsertId().toInt();
      else
        qDebug() << "createNew: " << le.text();

      wi.clear();
      setRowCount(numRow);
      setColumnCount(numCol);

      for (int i = 0; i < numRow; i++)
        for (int j = 0; j < numCol; j++)
          newCell(i, j, 0);

      isDirty = true;
    }

  delete dialog;
}

void tableTemplateWidget::deleteTemplate(void)
{
  if (templateId == 0)
    return;

  QMessageBox delTmp(QMessageBox::Question,
                     tr("Deleting template!"),
                     tr("Do you want delete template ?"),
                     QMessageBox::Yes | QMessageBox::No);

  int code = delTmp.exec();

  if (code == QMessageBox::No)
    return;

  QSqlQuery query;
  QSqlError le;

  query.prepare("DELETE FROM crossword.private_data WHERE _template = ?;");
    query.addBindValue(QVariant(templateId));
  query.exec();

  le = query.lastError();
  if (le.type() != QSqlError::NoError)
    qDebug() << "1. deleteTemplate: " << le.text();

  query.prepare("DELETE FROM crossword.grids WHERE _template = ?;");
    query.addBindValue(QVariant(templateId));
  query.exec();

  le = query.lastError();
  if (le.type() != QSqlError::NoError)
    qDebug() << "2. deleteTemplate: " << le.text();

  query.prepare("DELETE FROM crossword.templates WHERE _id = ?;");
    query.addBindValue(QVariant(templateId));
  query.exec();

  le = query.lastError();
  if (le.type() != QSqlError::NoError)
    qDebug() << "3. deleteTemplate: " << le.text();

  for (int i = 0; i < numCol; i++)
    removeColumn(0);

  setRowCount(0);

  sb->showMessage(tr("Template deleted"), 2000);

  // need for templateListWidget
  emit deletedFromDB(templateId);

  numCol = numRow = 0;
  templateId = 0;
  countWords = 0;
  wi.clear();
}

void tableTemplateWidget::setClickedCell(int row, int col)
{
  QTableWidgetItem *cell = this->item(row, col);

  if (cell)
    {
      if ( cell->data(Qt::UserRole).toInt() )
        updateCell(cell, 0);
      else
        updateCell(cell, 1);
    }

  if (symetricalMode)
    {
      QTableWidgetItem *mirrorCell = this->item(row, numCol-col-1);
      if (mirrorCell)
        {
          if ( mirrorCell->data(Qt::UserRole).toInt() )
            updateCell(mirrorCell, 0);
          else
            updateCell(mirrorCell, 1);
        }
    }

  isDirty = true;
}

void tableTemplateWidget::newCell(int row, int col, int value)
{
  QTableWidgetItem *cell = new QTableWidgetItem();

  if (cell)
    {
      cell->setFont(font);
      cell->setTextAlignment(Qt::AlignCenter);
      updateCell(cell, value);

      setItem(row, col, cell);
    }
}

void tableTemplateWidget::updateCell(QTableWidgetItem *cell, int value)
{
  cell->setData(Qt::UserRole, QVariant(value));

  if (value)
    cell->setBackgroundColor(fullCell);
  else
    cell->setBackgroundColor(emptyCell);
}

/*
 * Шукаємо слова в шаблоні та перетини між ними
 */
void tableTemplateWidget::scanTemplate(void)
{
  if (numRow == 0 && numCol == 0 && templateId != 0)
    {
      qDebug() << "scanTemplate: Template doesn't choiced!";
      return;
    }

  wi.clear();

  //стартуємо сканування з першого слова
  numWord = 0;

  scanHorizontal();
  scanVertical();

  int cc = 0, nw = 0, nw2 = 0;
  crossInfo *cross;

  // шукаємо інші слова з якими перетинаємося
  for (int i = 0; i < wi.count(); i++)
    {
      cc = wi[i]->crossCount;
      nw = wi[i]->numWord;

      for (int j = 0; j < cc; j++)
        {
          cross = wi[i]->cil[j];

          nw2 = findCrossedWord(cross->row, cross->col, nw);

          if (nw2 >= 0)
            cross->numWord2 = nw2;
          else
            qDebug() << "scanTemplate: another word for this crosses doesn't finded!!!";
        }
    }
}

/*
 * Шукаємо горизонтальні слова
 */
void tableTemplateWidget::scanHorizontal(void)
{
  int wordLength, countOfIntersection;
  int resrow = 0, rescol = 0, reswl = 0;
  QTableWidgetItem *cell = NULL;
  int value = -1;

  for (int i = 0; i < numRow; i++)
    {
      wordLength = countOfIntersection = 0;

      for (int j = 0; j < numCol; j++)
        {
          cell = item(i, j);
          value = cell->data(Qt::UserRole).toInt();

          if (value)
            {
              if (wordLength == 0)
                {
                  resrow = i;
                  rescol = j;
                }

              int intersection = typeOfIntersection(i, j);
              if (intersection)
                {
                  updateCell(cell, intersection);
                  countOfIntersection++;
                }

              wordLength++;
            }
          else
            {
              reswl = wordLength;
              wordLength = 0;
              saveResult(resrow, rescol, reswl, horizontal, countOfIntersection);
              countOfIntersection = 0;
            }
        }

      reswl = wordLength;
      saveResult(resrow, rescol, reswl, horizontal, countOfIntersection);
    }
}

/*
 * Шукаємо вертикальні слова
 */
void tableTemplateWidget::scanVertical(void)
{
  int wordLength, countOfIntersection;
  int resrow = 0, rescol = 0, reswl = 0;
  QTableWidgetItem *cell = NULL;
  int value = -1;

  for (int i = 0; i < numCol; i++)
    {
      wordLength = countOfIntersection = 0;

      for (int j = 0; j < numRow; j++)
        {
          cell = item(j, i);
          value = cell->data(Qt::UserRole).toInt();

          if (value)
            {
              if (wordLength == 0)
                {
                  resrow = j;
                  rescol = i;
                }

              int intersection = typeOfIntersection(j, i);
              if (intersection)
                {
                  updateCell(cell, intersection);
                  countOfIntersection++;
                }

              wordLength++;
            }
          else
            {
              reswl = wordLength;
              wordLength = 0;
              saveResult(resrow, rescol, reswl, vertical, countOfIntersection);
              countOfIntersection = 0;
            }
        }

      reswl = wordLength;
      saveResult(resrow, rescol, reswl, vertical, countOfIntersection);
    }
}

int tableTemplateWidget::typeOfIntersection(int row, int col)
{
  int toi = 0;
  QTableWidgetItem *cell = NULL;

  for (int i = 0; i < 4; i++)
    {
      cell = item(row + DRC[i][0], col + DRC[i][1]);

      if (cell)
        {
          if (cell->data(Qt::UserRole).toInt())
            toi = toi + int(pow(2, 4-(i+1)));
        }
    }

  if (toi)
    {
      for (int i = 0; i < 9; i++)
        if (intersections[i] == toi)
          return toi;
    }

  return 0;
}

void tableTemplateWidget::saveResult(int row, int col,  int length,
                                     int orient, int crossCount)
{
  wordInfo *word;
  crossInfo *cross;
  //int crossIndex = 0;
  int crossType = 0;

  if (length >= 3)
    {
      word = new wordInfo();
      word->numWord = numWord;
      word->row = row;
      word->col = col;
      word->length = length;
      word->orient = orient;
      word->crossCount = crossCount;

      if (orient == horizontal)
        {
          for (int i = 0; i < length; i++)
            {
              crossType = typeOfIntersection(row, col + i);

              if (crossType)
                {
                  //crossIndex++;

                  cross = new crossInfo();
                  cross->numWord = numWord;
                  cross->row = row;
                  cross->col = col + i;
                  //cross->crossIndex = crossIndex;
                  cross->crossPos = i;
                  cross->crossType = crossType;

                  word->cil.append(cross);
                }
            }
        }
      else
        {
          for (int i = 0; i < length; i++)
            {
              crossType = typeOfIntersection(row + i, col);

              if (crossType)
                {
                  //crossIndex++;

                  cross = new crossInfo();
                  cross->numWord = numWord;
                  cross->row = row + i;
                  cross->col = col;
                  //cross->crossIndex = crossIndex;
                  cross->crossPos = i;
                  cross->crossType = crossType;

                  word->cil.append(cross);
                }
            }
        }

      wi.append(word);

      // збільшуємо після кожного знайденого слова
      numWord++;
    }
}

int tableTemplateWidget::findCrossedWord(int row, int col, int numWord)
{
  int row2 = -1;
  int col2 = -1;
  int nw2 = -1;

  for (int i = 0; i < wi.count(); i++)
    {
      nw2 = wi[i]->numWord;

      if (numWord != nw2)
        {
          for (int j = 0; j < wi[i]->crossCount; j++)
            {
              row2 = wi[i]->cil[j]->row;
              col2 = wi[i]->cil[j]->col;

              if (row == row2 && col == col2)
                return nw2; // повертаємо порядковий номер слова з яким знайдено перетин
            }
        }
    }

  // перетин не знайдено
  return -1;
}


/*
 * Пошук слова по номеру рядка і стовпчика.
 * Може з часом замінити все на QMap і тоді подібні функції викинути
 */
int tableTemplateWidget::findWordByRC(int row, int col)
{
  int row2 = -1;
  int col2 = -1;

  for (int i = 0; i < wi.count(); i++)
    {
      row2 = wi[i]->row;
      col2 = wi[i]->col;

      if (row == row2 && col == col2)
        return wi[i]->numWord;
    }

  return -1;
}


void tableTemplateWidget::makeCrossword(void)
{
  if (templateId == 0)
    {
      qDebug() << tr("makeCrossword: Template dosen't choiced!");
      emit maked();

      return;
    }

  if (isDirty)
    {
      QMessageBox save(QMessageBox::Question,
                       tr("Template is changed!"),
                       tr("Template is changed, do you want save changes?"),
                       QMessageBox::Save | QMessageBox::Discard);

      int code = save.exec();

      if (code == QMessageBox::Save)
        saveToDB();
      else
        return;
    }

  if (mct)
    {
      mct->setData(wi);
      mct->startScanning();
      mct->start(QThread::LowestPriority);
      connect(mct, SIGNAL(finished()), this, SLOT(makeFinished()));
    }
}

void tableTemplateWidget::displayCrossword(void)
{
  int row, col, len;
  bool orient;
  QString text;

  QTableWidgetItem *cell = NULL;

  for (int i = 0; i < wi.count(); i++)
    {
      row = wi[i]->row;
      col = wi[i]->col;
      len = wi[i]->length;
      orient = wi[i]->orient;
      text = wi[i]->text;

      for (int j = 0; j < len; j++)
        {
          if (orient)
            cell = item(row+j, col);
          else
            cell = item(row, col+j);

          if (cell)
            {
              if (!text.isEmpty())
                cell->setText(QString(text[j]).toUpper());
            }
        }
    }
}

void tableTemplateWidget::printPreview(QPrinter *prn)
{
  if (templateId == 0 && !isMaked)
    return;

  printDoc.clear();
  QTextCursor cursor(&printDoc);

  int pageW = prn->pageRect().width();
  int pageH = prn->pageRect().height();
  int gapH = 20, gapV = 20;

  qreal leftM, rightM, topM, bottomM;
  qreal leftPoint, topPoint;
  prn->getPageMargins(&leftM, &topM, &rightM, &bottomM, QPrinter::Millimeter);

  font.setPointSize(14);
  font.setBold(false);

  int countRecords;

  QPainter painter;
  QImage imagePreview;
  int imageW = 0, imageH = 0;
  QSqlQuery query;

  query.prepare("SELECT _preview FROM crossword.templates WHERE _id = ?;");
  query.addBindValue(QVariant(templateId));
  query.exec();

  painter.begin(prn);
  {
    QString text("Шаблон № %1");
    text = text.arg(templateId);

    painter.setFont(font);
    QFontMetrics fm = painter.fontMetrics();

    leftPoint = leftM;
    topPoint = fm.height() + topM;
    painter.drawText(leftPoint, topPoint, text);

    QSqlError le = query.lastError();
    if (le.type() == QSqlError::NoError)
      {
        if (query.isActive() && query.isSelect())
          countRecords = query.size();
        else
          countRecords = -1;

        if (countRecords > 0)
          {
            int previewNo = query.record().indexOf("_preview");

            QByteArray _ba2;
            QBuffer buffer2(&_ba2);

            query.first();
            _ba2 = query.value(previewNo).toByteArray();

            buffer2.setBuffer(&_ba2);
            buffer2.open(QIODevice::ReadWrite | QIODevice::Unbuffered);

            imagePreview.load(&buffer2, "PNG");
            imageW = imagePreview.width();
            imageH = imagePreview.height();

            if (imageW > pageW || imageH > pageH)
              {
                imagePreview = imagePreview.scaled(QSize(pageW, pageW), Qt::KeepAspectRatio);

                imageW = imagePreview.width();
                imageH = imagePreview.height();
              }

            topPoint += gapH;
            painter.drawImage(QPoint(leftPoint, topPoint), imagePreview);

            //  doc.addResource(QTextDocument::ImageResource, QUrl("image"), QVariant(imagePreview));
            //	cursor.insertImage("image");

            buffer2.close();
          }
      }

    int x = imageW + gapV + leftM;
    int y = topPoint;
    int w = pageW - imageW - gapV - leftM - rightM;
    int h = imageH;

    QRect wordsRect(x, y, w, h);
    painter.drawRect(wordsRect);

    QString res;
    text = "%1.%2; ";

    for (int i = 0; i < wi.count(); i++)
      {
        if (wi[i]->text != "")
          res += text.arg(wi[i]->numWord+1).arg(wi[i]->text);
      }

    painter.drawText(wordsRect, Qt::TextWordWrap, res);
    //	cursor.insertHtml(res);

    x = leftM;
    y = imageH + topPoint + gapH;
    w = pageW - leftM - rightM;
    h = (pageH - y) - (fm.height() + gapH);

    QRect questionsRect(x, y, w, h);
    painter.drawRect(questionsRect);

    text = "<b>%1.</b>%2; ";

    prepareQuestions();

    font.setPointSize(8);
    font.setBold(false);
    printDoc.setDefaultFont(font);

    QRectF clip(0, 0, w, h);
    printDoc.setTextWidth(w);
    cursor.insertHtml(questionsV);
    cursor.insertHtml(questionsH);

    painter.save();
    painter.translate(x, y);
    painter.setFont(font);
    printDoc.drawContents(&painter, clip);
    painter.restore();
  }
  painter.end();
}

void tableTemplateWidget::makeFinished(void)
{
  displayCrossword();
  isMaked = true;

  prepareQuestions();

  emit maked();
}

void tableTemplateWidget::prepareQuestions(void)
{
  if (questionsH != "" && questionsV != "")
    return;

  QSqlQuery query;
  QSqlError le;

  query.prepare("SELECT _question FROM slovopedia.works WHERE _id = ?;");

  QString question, text = "%1.%2; ";
  int questionNo, i, countRecords;

  QMap<int, QString> questionMap;
  QMap<int, QString>::iterator it;

  questionsV = "<html><body>";
  questionsV += "<b>По вертикалі:</b><br>\n";
  for (i = 0; i < wi.count(); i++)
    {
      if (wi[i]->orient == vertical)
        {
          query.addBindValue(QVariant(wi[i]->wordId));
          query.exec();

          le = query.lastError();
          if (le.type() == QSqlError::NoError)
            {
              if (query.isActive() && query.isSelect())
                countRecords = query.size();
              else
                countRecords = -1;

              if (countRecords > 0)
                {
                  questionNo = query.record().indexOf("_question");
                  query.first();

                  question = query.value(questionNo).toString();
                  questionMap[wi[i]->numWord+1] = question;
                }
            }
        }
    }

  for (it = questionMap.begin(); it != questionMap.end(); ++it)
    questionsV += text.arg(it.key()).arg(it.value());

  questionMap.clear();
  questionsV += "</body></html>";

  questionsH = "\n\n<br><br><html><body>";
  questionsH += "<b>По горизонталі:</b><br>\n";
  for (i = 0; i < wi.count(); i++)
    {
      if (wi[i]->orient == horizontal)
        {
          query.addBindValue(QVariant(wi[i]->wordId));
          query.exec();

          le = query.lastError();
          if (le.type() == QSqlError::NoError)
            {
              if (query.isActive() && query.isSelect())
                countRecords = query.size();
              else
                countRecords = -1;

              if (countRecords > 0)
                {
                  questionNo = query.record().indexOf("_question");
                  query.first();

                  question = query.value(questionNo).toString();
                  questionMap[wi[i]->numWord+1] = question;
                }
            }
        }
    }

  for (it = questionMap.begin(); it != questionMap.end(); ++it)
    questionsH += text.arg(it.key()).arg(it.value());

  questionMap.clear();
  questionsH += "</body></html>";

  questionsDoc.setHtml(questionsV + questionsH);
}

void tableTemplateWidget::setStatusBar(QStatusBar *qsb)
{
  sb = qsb;
  connect(mct, SIGNAL(showStatusBarMessage(QString)), sb, SLOT(showMessage(QString)));
}

void tableTemplateWidget::setVocabulary(int vocabularyId)
{
  qDebug() << "Set vocabulary to the: " << vocabularyId;

  if (mct)
    mct->setVocabulary(vocabularyId);
}
