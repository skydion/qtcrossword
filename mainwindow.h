#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QTableWidgetSelectionRange>
#include <QListWidgetItem>
#include <QFile>
#include <QDataStream>
#include <QtSql>
#include <QMessageBox>
#include <QAction>
#include <QMap>
#include <QtGlobal>
#include <QPrintPreviewWidget>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#include "tabletemplatewidget.h"
#include "previewtemplatedelegate.h"
#include "previewcelldelegate.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString readStyleSheet(QString fileName);

public slots:
    void exit(void);

protected slots:
    void makedCrossword(void);
    void printCrossword(void);

protected:
    void changeEvent(QEvent *e);
    bool event(QEvent *e);
    void loadListPreview(void);

private:
    QPrinter *printer;

    QPrintPreviewDialog *prnPreview;
    QPrintPreviewWidget *printpreview;
    QSqlDatabase db;
    Ui::MainWindow *ui;

    QAction *actionNew, *actionSave, *actionDelete, *actionExit;
    QAction *actionScan, *actionMirror, *actionMake, *actionPrint;

    tableTemplateWidget *grid;

    PreviewTemplateDelegate *previewTemplate;
    PreviewCellDelegate *previewCell;
};

#endif // MAINWINDOW_H
