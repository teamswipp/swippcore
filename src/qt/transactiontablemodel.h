// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRANSACTIONTABLEMODEL_H
#define TRANSACTIONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QBasicTimer>
#include <QStringList>

class CWallet;
class TransactionTablePriv;
class TransactionRecord;
class WalletModel;

#ifndef USE_OLDSTYLE_DATE_SELECTION
#define MAX_TRANSACTIONS_PER_TICK 1000
#endif

// UI model for the transaction table of a wallet.
class TransactionTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TransactionTableModel(CWallet* wallet, WalletModel *parent = 0);
    ~TransactionTableModel();

    enum ColumnIndex {
        Status = 0,
        Date = 1,
        Type = 2,
        ToAddress = 3,
        Amount = 4
    };

    // Roles to get specific information from a transaction row.
    // These are independent of column.
    enum RoleIndex {
        TypeRole = Qt::UserRole, // Type of transaction
        DateRole,                // Date and time this transaction was created
        LongDescriptionRole,     // Long description (HTML format)
        AddressRole,             // Address of transaction
        LabelRole,               // Label of address related to transaction
        AmountRole,              // Net amount of transaction
        TxIDRole,                // Unique identifier
        ConfirmedRole,           // Is transaction confirmed?
        FormattedAmountRole,     // Formatted amount, without brackets when unconfirmed
        StatusRole,              // Transaction status (TransactionRecord::Status)
        TransactionOrderPosRole  // Position in ordered transaction list
    };

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;

    int getWalletSize() const;

private:
    int lowerIndex;
    int upperIndex;
    int fetchedRows;

    CWallet* wallet;
    WalletModel *walletModel;
    QStringList columns;
    TransactionTablePriv *priv;

    void updateCells(enum ColumnIndex column);
    QString lookupAddress(const std::string &address, bool tooltip) const;
    QVariant addressColor(const TransactionRecord *wtx) const;
    QString formatTxStatus(const TransactionRecord *wtx) const;
    QString formatTxDate(const TransactionRecord *wtx) const;
    QString formatTxType(const TransactionRecord *wtx) const;
    QString formatTxToAddress(const TransactionRecord *wtx, bool tooltip) const;
    QString formatTxAmount(const TransactionRecord *wtx, bool showUnconfirmed=true) const;
    QString formatTooltip(const TransactionRecord *rec) const;
    QVariant txStatusDecoration(const TransactionRecord *wtx) const;
    QVariant txAddressDecoration(const TransactionRecord *wtx) const;

signals:
    void updated();

public slots:
    void updateTransaction(const QString &hash, int status);
    void updateConfirmations();
    void updateDisplayUnit();

    friend class TransactionTablePriv;
};

#endif // TRANSACTIONTABLEMODEL_H
