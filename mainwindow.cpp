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
    grid = NULL;
//    printpreview = NULL;
    previewTemplate = NULL;
    previewCell = NULL;

    db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("su112per");
    db.open();

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

//	actionPrint = new QAction(QIcon(":/icons/preview16.png"), QString(tr("Print")), this);
//	connect(actionPrint, SIGNAL(triggered(bool)), this, SLOT(printCrossword()));

	actionExit = new QAction(QIcon(":/icons/x16.png"), QString(tr("Exit")), this);
	connect(actionExit, SIGNAL(triggered()), this, SLOT(exit()));


	QMenu *menuFile = new QMenu(tr("File"), this);
	ui->menuBar->addMenu(menuFile);
	menuFile->addAction(actionNew);
	menuFile->addAction(actionSave);
	menuFile->addAction(actionDelete);
//	menuFile->addAction(actionPrint);
	menuFile->addSeparator();
	menuFile->addAction(actionExit);

	ui->mainToolBar->addAction(actionNew);
	ui->mainToolBar->addAction(actionSave);
	ui->mainToolBar->addAction(actionDelete);
//	ui->mainToolBar->addAction(actionPrint);

    // Edit menu
	ui->mainToolBar->addSeparator();

	QMenu *menuEdit = new QMenu(tr("Edit"), this);
	ui->menuBar->addMenu(menuEdit);
//	actionScan = new QAction(QIcon(":/icons/search16.png"), QString(tr("Scan")), this);
//	connect(actionScan, SIGNAL(triggered()), grid, SLOT(scanTemplate()));
//	ui->mainToolBar->addAction(actionScan);
//	menuEdit->addAction(actionScan);

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

//	grid->setStyleSheet(readStyleSheet("d:\\Crossword\\gui\\qribbon.qss"));
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

//	printpreview = new QPrintPreviewWidget(printer, this);
//
//	if (printpreview)
//	{
//	    printpreview->fitInView();
//	    printpreview->show();
//
//	    ui->horizontalLayout_8->addWidget(printpreview);
//	    connect(printpreview, SIGNAL(paintRequested(QPrinter*)), grid, SLOT(printPreview(QPrinter*)));
//	}
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

    QTextDocument *d = grid->getDocument();
    ui->textBrowser->setDocument(d);
}

//void MainWindow::printCrossword(void)
//{
//    printpreview->updatePreview();

//    QPrintDialog printDialog(printer, printpreview);
//
//    if (printDialog.exec() == QDialog::Accepted)
//    {
//	qDebug() << printpreview->numPages();
//	printpreview->print();
//    }
//}

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
