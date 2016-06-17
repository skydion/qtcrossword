#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  actionNew = actionSave = actionDelete = actionExit = NULL;
  actionScan = actionMirror = actionMake = NULL;
  actionPrint = NULL;
  vcb = NULL;
  grid = NULL;
  printpreview = NULL;
  previewTemplate = NULL;
  previewCell = NULL;

  //qDebug() << db.drivers();
  db = QSqlDatabase::addDatabase("QMYSQL");

  db.setHostName("localhost");
  db.setDatabaseName("crossword");
  db.setUserName("taras");
  db.setPassword("su112per");
  bool ok = db.open();

  if (!ok)
  {
    qDebug() << db.lastError().text();

    QMessageBox msgBox;
    msgBox.setText(tr("Database open error!"));
    msgBox.exec();
  }

  grid = new tableTemplateWidget();

  if (grid)
  {
    // File menu
    actionNew = new QAction(QIcon(":/icons/new16.png"), QString(tr("New")), this);
    connect(actionNew, SIGNAL(triggered()), grid, SLOT(createNew()));

    actionSave = new QAction(QIcon(":/icons/save16.png"), QString(tr("Save")), this);
    connect(actionSave, SIGNAL(triggered()), grid, SLOT(saveToDB()));

    actionDelete = new QAction(QIcon(":/icons/delete16.png"), QString(tr("Delete")), this);
    connect(actionDelete, SIGNAL(triggered()), grid, SLOT(deleteTemplate()));

    actionPrint = new QAction(QIcon(":/icons/preview16.png"), QString(tr("Print")), this);
    connect(actionPrint, SIGNAL(triggered(bool)), this, SLOT(printCrossword()));

    actionExit = new QAction(QIcon(":/icons/x16.png"), QString(tr("Exit")), this);
    connect(actionExit, SIGNAL(triggered()), this, SLOT(exit()));


    QMenu *menuFile = new QMenu(tr("File"), this);
    ui->menuBar->addMenu(menuFile);
    menuFile->addAction(actionNew);
    menuFile->addAction(actionSave);
    menuFile->addAction(actionDelete);
    menuFile->addAction(actionPrint);
    menuFile->addSeparator();
    menuFile->addAction(actionExit);

    ui->mainToolBar->addAction(actionNew);
    ui->mainToolBar->addAction(actionSave);
    ui->mainToolBar->addAction(actionDelete);
    ui->mainToolBar->addAction(actionPrint);

    // Edit menu
    ui->mainToolBar->addSeparator();

    QMenu *menuEdit = new QMenu(tr("Edit"), this);
    ui->menuBar->addMenu(menuEdit);
    //  actionScan = new QAction(QIcon(":/icons/search16.png"), QString(tr("Scan")), this);
    //  connect(actionScan, SIGNAL(triggered()), grid, SLOT(scanTemplate()));
    //  ui->mainToolBar->addAction(actionScan);
    //  menuEdit->addAction(actionScan);

    actionMirror = new QAction(QIcon(":/icons/favorites16.png"), QString(tr("Mirror")), this);
    actionMirror->setCheckable(true);
    actionMirror->setChecked(true);
    connect(actionMirror, SIGNAL(toggled(bool)), grid, SLOT(setSymetricalMode(bool)));
    ui->mainToolBar->addAction(actionMirror);
    menuEdit->addAction(actionMirror);

    actionMake = new QAction(QIcon(":/icons/make.png"), QString(tr("Build crossword")), this);
    actionMake->setCheckable(true);
    connect(actionMake, SIGNAL(toggled(bool)), grid, SLOT(makeCrossword()));
    ui->mainToolBar->addAction(actionMake);
    menuEdit->addAction(actionMake);

    vcb = new QComboBox(this);
    if (vcb)
    {
      QFont font;
      font.setPointSize(10);
      //font.setBold(true);

      vcb->setFixedSize(350, 32);
      vcb->setMaxVisibleItems(20);
      vcb->setFont(font);

      vcb->addItem("УСЕ (Універсальний словник-енциклопедія)", QVariant(29));
      vcb->addItem("Орфографічний словник української мови", QVariant(35));
      vcb->addItem("Фразеологічний словник української мови", QVariant(49));
      vcb->addItem("Словник синонімів Полюги", QVariant(31));
      vcb->addItem("Словник синонімів Караванського", QVariant(41));
      vcb->addItem("Словник іншомовних слів", QVariant(36));
      vcb->addItem("Словник іншомовних слів Мельничука", QVariant(42));
      vcb->addItem("Словник англіцизмів", QVariant(46));
      vcb->addItem("Eкономічна енциклопедія", QVariant(38));
      vcb->addItem("Словник мови Стуса", QVariant(27));
      vcb->addItem("Словник іншомовних соціокультурних термінів", QVariant(39));
      vcb->addItem("Енциклопедія політичної думки", QVariant(40));
      vcb->addItem("Словник церковно-обрядової термінології", QVariant(43));
      vcb->addItem("Архітектура і монументальне мистецтво", QVariant(44));
      vcb->addItem("Словник-антисуржик", QVariant(45));
      vcb->addItem("Словник термінів, уживаних у чинному Законодавстві України", QVariant(48));
      vcb->addItem("Словник бюджетної термінології", QVariant(50));
      vcb->addItem("Термінологічний словник з економіки праці", QVariant(51));
      vcb->addItem("Глосарій термінів Фондового ринку", QVariant(52));
      vcb->addItem("Моделювання економіки", QVariant(53));
      vcb->addItem("Власні імена людей", QVariant(54));
      vcb->addItem("Словар українського сленгу", QVariant(57));
      vcb->addItem("Музичні терміни", QVariant(58));
      vcb->addItem("Тлумачний словник з інформатики та інформаційних систем для економістів", QVariant(59));
      vcb->addItem("Управління якістю", QVariant(61));

      ui->mainToolBar->insertWidget(actionMake, vcb);
      connect(vcb, SIGNAL(currentIndexChanged(int)), this, SLOT(setVocabulary(int)));
      setVocabulary(vcb->currentIndex());
    }

    //grid->setStyleSheet(readStyleSheet("/home/taras/Projects/qtCrossword/qribbon.qss"));
    grid->setSymetricalMode(true);
    ui->horizontalLayout->addWidget(grid);
    ui->listWidget->setTemplateGrid(grid);

    grid->setStatusBar(ui->statusBar);
    grid->setDB(&db);

    connect(grid, SIGNAL(maked()), this, SLOT(makedCrossword()));
  }

  previewTemplate = new PreviewTemplateDelegate();

  if (previewTemplate)
    ui->listWidget->setItemDelegate(previewTemplate);

  loadListPreview();

  previewCell = new PreviewCellDelegate();
  ui->tableView->setItemDelegate(previewCell);
  ui->tableView->setModel( grid->model() );
  ui->tableView->horizontalHeader()->setDefaultSectionSize(40);
  ui->tableView->verticalHeader()->setDefaultSectionSize(40);
  ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  printer = new QPrinter();

  if (printer)
  {
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Portrait);
    printer->setFullPage(true);
    printer->setPageMargins(20.0, 15.0, 15.0, 15.0, QPrinter::Millimeter);
    printer->newPage();

    prnPreview = new QPrintPreviewDialog(printer, this);
    connect(prnPreview, SIGNAL(paintRequested(QPrinter*)), grid, SLOT(printPreview(QPrinter*)));
    ui->horizontalLayout_8->addWidget(prnPreview);

    printpreview = new QPrintPreviewWidget(printer, this);

    if (printpreview)
    {
      printpreview->fitInView();
      printpreview->show();

      ui->horizontalLayout_8->addWidget(printpreview);
      connect(printpreview, SIGNAL(paintRequested(QPrinter*)), grid, SLOT(printPreview(QPrinter*)));
    }
  }
}

MainWindow::~MainWindow()
{
  db.close();

  delete ui;

  if (grid)
    delete grid;

  if (previewTemplate)
    delete previewTemplate;

  if (previewCell)
    delete previewCell;
}

bool MainWindow::event(QEvent *e)
{
  switch (e->type()) {
    case QEvent::Close:
      exit();
      break;

    default:
      break;
  }

  return QMainWindow::event(e);

}

void MainWindow::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);

  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;

    default:
      break;
  }
}

void MainWindow::exit()
{
  bool dirty = grid->checkDirty();

  if (dirty)
  {
    QMessageBox save(QMessageBox::Question,
                     tr("Template is changed!"),
                     tr("Template is changed, do you want save changes?"),
                     QMessageBox::Save | QMessageBox::Discard);

    int code = save.exec();

    if (code == QMessageBox::Save)
      actionSave->activate(QAction::Trigger);
  }

  close();
}

QString MainWindow::readStyleSheet(QString fileName)
{
  QString css;
  QFile file(fileName);

  if (file.open(QIODevice::ReadOnly))
    css = QString(file.readAll());

  file.close();

  return css;
}

void MainWindow::makedCrossword(void)
{
  actionMake->setDisabled(false);
  actionMake->setChecked(false);

  ui->textBrowser->setDocument(grid->getQuestionsDocument());
}

void MainWindow::printCrossword(void)
{
  printpreview->updatePreview();

  QPrintDialog printDialog(printer, printpreview);

  if (printDialog.exec() == QDialog::Accepted)
  {
    qDebug() << printpreview->pageCount();
    printpreview->print();
  }
}

void MainWindow::loadListPreview(void)
{
  QListWidgetItem *lw;
  QImage *imagePreview;
  QImage scaledPreview;

  QSize s(200, 200); // = ui->listWidget->sizeHint();

  QSqlQuery query;
  query.prepare("SELECT _id, _preview FROM crossword.templates;");
  query.exec();

  int countRecord;
  QSqlError le = query.lastError();
  if (le.type() == QSqlError::NoError)
  {
    if (query.isActive() && query.isSelect())
      countRecord = query.size();
    else
      countRecord= -1;

    if (countRecord> 0)
    {
      int idNo = query.record().indexOf("_id");
      int previewNo = query.record().indexOf("_preview");

      int _id;
      QByteArray _ba2;
      QBuffer buffer2(&_ba2);

      while (query.next())
      {
        _id = query.value(idNo).toInt();
        _ba2 = query.value(previewNo).toByteArray();

        buffer2.setBuffer(&_ba2);
        buffer2.open(QIODevice::ReadWrite | QIODevice::Unbuffered);

        imagePreview = new QImage();
        imagePreview->load(&buffer2, "PNG");

        lw = new QListWidgetItem(ui->listWidget);

        if (lw)
        {
          if (imagePreview)
          {
            scaledPreview = imagePreview->scaled(s, Qt::KeepAspectRatio, Qt::FastTransformation);

            lw->setData(Qt::UserRole, QVariant(_id));
            lw->setData(Qt::DecorationRole, QVariant(scaledPreview));

            ui->listWidget->addItem(lw);
          }
        }

        buffer2.close();
      }
    }
  }
  else
    qDebug() << "loadListPreview: " << le.text();
}

void MainWindow::setVocabulary(int index)
{
  int vocId = vcb->itemData(index, Qt::UserRole).toInt();
  grid->setVocabulary(vocId);
}
