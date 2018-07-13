// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRANSACTIONFILTERPROXY_H
#define TRANSACTIONFILTERPROXY_H

#include <QSortFilterProxyModel>
#include <QDateTime>

// Filter the transaction list according to pre-specified rules
class TransactionFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TransactionFilterProxy(QObject *parent = 0);

#ifdef USE_OLDSTYLE_DATE_SELECTION
    static const QDateTime MIN_DATE;
    static const QDateTime MAX_DATE;
#endif

    static const quint32 ALL_TYPES = 0xFFFFFFFF;
    static quint32 TYPE(int type) { return 1<<type; }

#ifdef USE_OLDSTYLE_DATE_SELECTION
    void setDateRange(const QDateTime &from, const QDateTime &to);
#else
    void setRange(int min, int max);
#endif

    void setAddressPrefix(const QString &addrPrefix);
    void setTypeFilter(quint32 modes);
    void setMinAmount(qint64 minimum);
    void setLimit(int limit);
    void setShowInactive(bool showInactive);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

private:
#ifdef USE_OLDSTYLE_DATE_SELECTION
    QDateTime dateFrom;
    QDateTime dateTo;
#else
    int orderPosMin;
    int orderPosMax;
#endif
    QString addrPrefix;
    quint32 typeFilter;
    qint64 minAmount;
    int limitRows;
    bool showInactive;
};

#endif // TRANSACTIONFILTERPROXY_H
