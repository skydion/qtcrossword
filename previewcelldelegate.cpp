#include "previewcelldelegate.h"

PreviewCellDelegate::PreviewCellDelegate(QWidget *parent) :
  QStyledItemDelegate(parent)
{
  font.setFamily("Tahoma");
  font.setPointSize(8);
  font.setBold(true);
}

void PreviewCellDelegate::paint(QPainter *painter,
				const QStyleOptionViewItem &option,
				const QModelIndex &index) const
{
  Q_ASSERT(index.isValid());

  QColor color;
  QStyleOptionViewItemV4 myOption = option;
  QString text;

  //    myOption.displayAlignment = Qt::AlignLeft | Qt::AlignBottom;
  myOption.displayAlignment = Qt::AlignCenter;
  initStyleOption(&myOption, index);

  int value = index.data(Qt::UserRole).toInt();
  if (value != 0 && value != 1)
    text = QVariant(value).toString();
  else
    text = "";

  switch (value)
    {
    case 0:
      color = Qt::darkRed;
      break;

    case 1:
    case 2:
    case 4:
    case 8:
      color = Qt::green;
      break;

    case 3:
      color = Qt::blue;
      break;

    case 6:
      color = Qt::darkBlue;
      break;

    case 9:
      color = Qt::yellow;
      break;

    case 12:
      color = Qt::darkYellow;
      break;

    case 7:
      color = Qt::cyan;
      break;

    case 13:
      color = Qt::darkCyan;
      break;

    case 11:
      color = Qt::magenta;
      break;

    case 14:
      color = Qt::darkMagenta;
      break;

    case 15:
      color = Qt::red;
      break;

    case 5:
    case 10:
      color = Qt::cyan;
      break;
    }

  painter->setFont(font);
  painter->fillRect(myOption.rect, color);
  //    painter->drawRect(myOption.rect);
  painter->drawText(myOption.rect, Qt::AlignLeft | Qt::AlignTop, text);
}

QSize PreviewCellDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
  Q_ASSERT(index.isValid());

  return option.rect.size();
}
