#include "proxytabletolist.h"

proxyTableToList::proxyTableToList(QObject *parent) :
    QAbstractProxyModel(parent)
{
}

QModelIndex proxyTableToList::mapFromSource(const QModelIndex &sourceIndex) const
{
    return QModelIndex();
}

QModelIndex proxyTableToList::mapToSource(const QModelIndex &proxyIndex) const
{
    return QModelIndex();
}
