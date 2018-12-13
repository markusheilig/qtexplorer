#ifndef SORTBYNAMEPROXY_H
#define SORTBYNAMEPROXY_H

#include <QSortFilterProxyModel>

class SortByNameProxy : public QSortFilterProxyModel
{
public:
    SortByNameProxy();

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif // SORTBYNAMEPROXY_H
