#ifndef PREVIEWCELLDELEGATE_H
#define PREVIEWCELLDELEGATE_H

#include <QPainter>
#include <QStyledItemDelegate>
#include <QFont>

class PreviewCellDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  explicit PreviewCellDelegate(QWidget *parent = 0);

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

private:
  QFont font;
};

#endif // PREVIEWCELLDELEGATE_H
