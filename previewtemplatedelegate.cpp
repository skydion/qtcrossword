#include "previewtemplatedelegate.h"

PreviewTemplateDelegate::PreviewTemplateDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void PreviewTemplateDelegate::paint(QPainter *painter,
				    const QStyleOptionViewItem &option,
				    const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

//    QStyleOptionViewItemV4 myOption = option;
//    myOption.displayAlignment = Qt::AlignLeft | Qt::AlignBottom;
//    initStyleOption(&myOption, index);

//    QBrush brush = index.data(Qt::BackgroundRole).value<QBrush>();
//    QImage imagePreview = brush.textureImage();

    if(option.state & QStyle::State_Selected){
      painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
    }

//    painter->drawImage(QPoint(0,0), imagePreview);

    QStyledItemDelegate::paint(painter, option, index);
}

QSize PreviewTemplateDelegate::sizeHint(const QStyleOptionViewItem &option,
					const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());
//
//    QByteArray ba = index.data(Qt::DecorationRole).toByteArray();
//    QBuffer buffer(&ba);
//
//    buffer.setBuffer(&ba);
//    buffer.open(QIODevice::ReadWrite | QIODevice::Unbuffered);
//
//    QImage *imagePreview = new QImage();
//    imagePreview->load(&buffer, "PNG");
//
//    return imagePreview->size();
    return QSize(200, 200);
}
