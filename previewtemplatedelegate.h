#ifndef PREVIEWTEMPLATEDELEGATE_H
#define PREVIEWTEMPLATEDELEGATE_H

#include <QStringListModel>
#include <QStyleOptionFocusRect>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QBuffer>
#include <QBrush>
#include <QVariant>

class PreviewTemplateDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  explicit PreviewTemplateDelegate(QObject *parent = 0);

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:
};

#endif // PREVIEWTEMPLATEDELEGATE_H
