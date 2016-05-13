#ifndef PROXYTABLETOLIST_H
#define PROXYTABLETOLIST_H

#include <QAbstractProxyModel>

class proxyTableToList : public QAbstractProxyModel
{
  Q_OBJECT
public:
  explicit proxyTableToList(QObject *parent = 0);

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
  QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

signals:

public slots:

};

#endif // PROXYTABLETOLIST_H
