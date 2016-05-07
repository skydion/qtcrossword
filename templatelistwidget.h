#ifndef TEMPLATELISTWIDGET_H
#define TEMPLATELISTWIDGET_H

#include <QListWidget>
#include <QResizeEvent>
#include <QSize>

#include "tabletemplatewidget.h"

class TemplateListWidget : public QListWidget
{
Q_OBJECT
public:
    explicit TemplateListWidget(QWidget *parent = 0);

    void setTemplateGrid(tableTemplateWidget *ttw);

protected:
    tableTemplateWidget *grid;

protected:

protected slots:
    void updateGrid(QListWidgetItem *item);
    void updateList(int id);
    void deleteItem(int id);

signals:

public slots:
};

#endif // TEMPLATELISTWIDGET_H
