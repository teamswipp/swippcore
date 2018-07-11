// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINUNITS_H
#define BITCOINUNITS_H

#include <QString>
#include <QAbstractListModel>

class BitcoinUnits: public QAbstractListModel
{
    Q_OBJECT

public:
    explicit BitcoinUnits(QObject *parent);

    enum Unit
    {
        BTC,
        mBTC,
        uBTC
    };

    // Get list of units, for drop-down box
    static QList<Unit> availableUnits();

    // Is unit ID valid?
    static bool valid(int unit);

    static QString name(int unit);
    static QString description(int unit);

    // Number of Satoshis (1e-8) per unit
    static qint64 factor(int unit);

    // Number of amount digits (to represent max number of coins)
    static int amountDigits(int unit);

    // Number of decimals left
    static int decimals(int unit);

    static QString format(int unit, qint64 amount, bool plussign=false);
    static QString formatWithUnit(int unit, qint64 amount, bool plussign=false);

    // Parse string to coin amount
    static bool parse(int unit, const QString &value, qint64 *val_out);

    enum RoleIndex {
        UnitRole = Qt::UserRole
    };

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    QList<BitcoinUnits::Unit> unitlist;
};

typedef BitcoinUnits::Unit BitcoinUnit;

#endif // BITCOINUNITS_H
