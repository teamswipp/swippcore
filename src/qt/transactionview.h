#ifndef TRANSACTIONVIEW_H
#define TRANSACTIONVIEW_H

#include "rangeslider.h"

#include <QWidget>

class WalletModel;
class TransactionFilterProxy;

QT_BEGIN_NAMESPACE
class QTableView;
class QComboBox;
class QLineEdit;
class QModelIndex;
class QMenu;
class QFrame;
class QDateTimeEdit;
QT_END_NAMESPACE

/** Widget showing the transaction list for a wallet, including a filter row.
    Using the filter row, the user can view or export a subset of the transactions.
  */
class TransactionView : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionView(QWidget *parent = 0);
    void setModel(WalletModel *model);

#ifdef USE_OLDSTYLE_DATE_SELECTION
    // Date ranges for filter
    enum DateEnum
    {
        All,
        Today,
        ThisWeek,
        ThisMonth,
        LastMonth,
        ThisYear,
        Range
    };
#endif

private:
    WalletModel *model;
    TransactionFilterProxy *transactionProxyModel;
    QTableView *transactionView;

#ifdef USE_OLDSTYLE_DATE_SELECTION
    QComboBox *dateWidget;
#else
    RangeSlider *dateWidget;
#endif

    QComboBox *typeWidget;
    QLineEdit *addressWidget;
    QLineEdit *amountWidget;

    QMenu *contextMenu;

    QFrame *dateRangeWidget;
    QDateTimeEdit *dateFrom;
    QDateTimeEdit *dateTo;

    void calculateRange();

#ifdef USE_OLDSTYLE_DATE_SELECTION
    QWidget *createDateRangeWidget();
#endif

private slots:
    void contextualMenu(const QPoint &);

#ifdef USE_OLDSTYLE_DATE_SELECTION
    void dateRangeChanged();
#endif

    void showDetails();
    void copyAddress();
    void editLabel();
    void copyLabel();
    void copyAmount();
    void copyTxID();

signals:
    void doubleClicked(const QModelIndex&);

public slots:
#ifdef USE_OLDSTYLE_DATE_SELECTION
    void chooseDate(int idx);
#else
    void chooseRangeSelection(int aMin, int aMax);
    void chooseRange();
#endif

    void setDisabledIfNotSyncing();

    void chooseType(int idx);
    void changedPrefix(const QString &prefix);
    void changedAmount(const QString &amount);
    void exportClicked();
    void focusTransaction(const QModelIndex&);

};

#endif // TRANSACTIONVIEW_H
