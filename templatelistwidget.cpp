#include "templatelistwidget.h"

TemplateListWidget::TemplateListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setResizeMode(QListView::Adjust);

    connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(updateGrid(QListWidgetItem*)));
}

void TemplateListWidget::updateGrid(QListWidgetItem *item)
{
    int templateId = item->data(Qt::UserRole).toInt();
    grid->setTemplateId(templateId);
    grid->loadFromDB();
}

void TemplateListWidget::updateList(int id)
{
    int countRecord = -1;

    QSqlQuery query;
    query.prepare("SELECT _preview FROM crossword.templates WHERE _id = ?;");
    query.addBindValue(QVariant(id));
    query.exec();

    QSqlError le = query.lastError();
    if (le.type() == QSqlError::NoError)
    {
    	int previewNo = query.record().indexOf("_preview");

    	if (query.isActive() && query.isSelect())
    	    countRecord = query.size();
    	else
    	    countRecord = -1;

    	if (countRecord > 0)
    	{
    	    QByteArray _ba2;
    	    QBuffer buffer2(&_ba2);

    	    query.next();

    	    _ba2 = query.value(previewNo).toByteArray();

    	    buffer2.setBuffer(&_ba2);
    	    buffer2.open(QIODevice::ReadWrite | QIODevice::Unbuffered);

    	    QImage *imagePreview = new QImage();
	    imagePreview->load(&buffer2, "PNG");

	    QListWidgetItem *lwi = NULL, *cur;
	    for (int i = 0; i < count(); i++)
	    {
		cur = item(i);
		if (cur->data(Qt::UserRole).toInt() == id)
		    lwi = cur;
	    }

	    if (lwi)
	    {
		if (imagePreview)
		{
		    QImage scaledPreview = imagePreview->scaled(sizeHint(), Qt::KeepAspectRatio, Qt::FastTransformation);
		    lwi->setData(Qt::DecorationRole, QVariant(scaledPreview));
		}
	    }
	    else
	    {
		lwi = new QListWidgetItem(this);

		if (lwi)
		{
		    if (imagePreview)
		    {
			QImage scaledPreview = imagePreview->scaled(sizeHint(), Qt::KeepAspectRatio, Qt::FastTransformation);
			lwi->setData(Qt::UserRole, QVariant(id));
			lwi->setData(Qt::DecorationRole, QVariant(scaledPreview));
		    }

		    addItem(lwi);
		}
	    }

	    buffer2.close();
    	}
    }
    else
    	qDebug() << "updateList: " << le.text();
}

void TemplateListWidget::setTemplateGrid(tableTemplateWidget *ttw)
{
    grid = ttw;
    connect(grid, SIGNAL(savedToDB(int)), this, SLOT(updateList(int)));
    connect(grid, SIGNAL(deletedFromDB(int)), this, SLOT(deleteItem(int)));
}

void TemplateListWidget::deleteItem(int id)
{
    QListWidgetItem *cur, *lwi;

    int cnt = this->count();

    for (int i = 0; i < cnt; i++)
    {
	cur = item(i);

	if (cur->data(Qt::UserRole).toInt() == id)
	{
	    lwi = takeItem(i);

	    if (lwi)
		delete lwi;

	    return;
	}
    }
}
