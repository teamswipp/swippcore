// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QDateTime>
#include <cstdlib>
#include <climits>

#include "transactionfilterproxy.h"
#include "transactiontablemodel.h"
#include "transactionrecord.h"

#ifdef USE_OLDSTYLE_DATE_SELECTION
// Earliest date that can be represented (far in the past)
const QDateTime TransactionFilterProxy::MIN_DATE = QDateTime::fromTime_t(0);
// Last date that can be represented (far in the future)
const QDateTime TransactionFilterProxy::MAX_DATE = QDateTime::fromTime_t(0xFFFFFFFF);
#endif

TransactionFilterProxy::TransactionFilterProxy(QObject *parent) :
    QSortFilterProxyModel(parent),
#ifdef USE_OLDSTYLE_DATE_SELECTION
    dateFrom(MIN_DATE),
    dateTo(MAX_DATE),
#else
    orderPosMin(INT_MIN),
    orderPosMax(INT_MAX),
#endif
    addrPrefix(),
    typeFilter(ALL_TYPES),
    minAmount(0),
    limitRows(-1),
    showInactive(true)
{
}

bool TransactionFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    int type = index.data(TransactionTableModel::TypeRole).toInt();
    QDateTime datetime = index.data(TransactionTableModel::DateRole).toDateTime();
    QString address = index.data(TransactionTableModel::AddressRole).toString();
    QString label = index.data(TransactionTableModel::LabelRole).toString();
    qint64 amount = llabs(index.data(TransactionTableModel::AmountRole).toLongLong());
    int status = index.data(TransactionTableModel::StatusRole).toInt();

    // This effectively reverses the transaction position to take into account the reversed ordering of the list
    int orderPos = ((TransactionTableModel *) sourceModel())->getWalletSize() -
                   index.data(TransactionTableModel::TransactionOrderPosRole).toInt();

    if(!showInactive && (status == TransactionStatus::Conflicted || status == TransactionStatus::NotAccepted))
        return false;
    else if(!(TYPE(type) & typeFilter))
        return false;
    else if (!address.contains(addrPrefix, Qt::CaseInsensitive) && !label.contains(addrPrefix, Qt::CaseInsensitive))
        return false;
    else if(amount < minAmount)
        return false;
#ifdef USE_OLDSTYLE_DATE_SELECTION
    else if(datetime < dateFrom || datetime > dateTo)
        return false;
#else
    else if ((orderPos < orderPosMin || orderPos > orderPosMax) && orderPos >= 0)
        return false;
    else if (orderPosMin > 0 && orderPos < 0)
        return false;
#endif

    return true;
}

#ifdef USE_OLDSTYLE_DATE_SELECTION
void TransactionFilterProxy::setDateRange(const QDateTime &from, const QDateTime &to)
{
    this->dateFrom = from;
    this->dateTo = to;
    invalidateFilter();
}
#else
void TransactionFilterProxy::setRange(int min, int max)
{
    this->orderPosMin = min * MAX_TRANSACTIONS_PER_TICK;
    this->orderPosMax = max * MAX_TRANSACTIONS_PER_TICK;
    invalidateFilter();
}
#endif

void TransactionFilterProxy::setAddressPrefix(const QString &addrPrefix)
{
    this->addrPrefix = addrPrefix;
    invalidateFilter();
}

void TransactionFilterProxy::setTypeFilter(quint32 modes)
{
    this->typeFilter = modes;
    invalidateFilter();
}

void TransactionFilterProxy::setMinAmount(qint64 minimum)
{
    this->minAmount = minimum;
    invalidateFilter();
}

void TransactionFilterProxy::setLimit(int limit)
{
    this->limitRows = limit;
    invalidateFilter();
}

void TransactionFilterProxy::setShowInactive(bool showInactive)
{
    this->showInactive = showInactive;
    invalidateFilter();
}

int TransactionFilterProxy::rowCount(const QModelIndex &parent) const
{
    if(limitRows != -1)
        return std::min(QSortFilterProxyModel::rowCount(parent), limitRows);
    else
        return QSortFilterProxyModel::rowCount(parent);
}

void TransactionFilterProxy::invalidateFilter()
{
	QSortFilterProxyModel::invalidateFilter();
}
