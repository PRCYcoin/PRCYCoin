// Copyright (c) 2011-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_HISTORYPAGE_H
#define BITCOIN_QT_HISTORYPAGE_H

#include "guiutil.h"
#include "togglebutton.h"
#include "transactiontablemodel.h"

#include <QAbstractTableModel>
#include <QDialog>
#include <QHeaderView>
#include <QItemSelection>
#include <QKeyEvent>
#include <QMenu>
#include <QPoint>
#include <QSettings>
#include <QTimeEdit>
#include <QVariant>
#include <QSizeGrip>

class WalletModel;

namespace Ui
{
class HistoryPage;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
class QDataWidgetMapper;
QT_END_NAMESPACE

/** Dialog for options page */
class HistoryPage : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryPage(QWidget* parent = 0);
    ~HistoryPage();
    void setModel(WalletModel* model);

public Q_SLOTS:
    void updateFilter();
    void syncTime(QDateTimeEdit* calendar, QTimeEdit* clock);

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private:
    Ui::HistoryPage* ui;
    GUIUtil::TableViewLastColumnResizingFixer* columnResizingFixer;
    WalletModel* model;
    QAbstractTableModel* tableModel;

    QTimeEdit* timeEditTo;
    QTimeEdit* timeEditFrom;

    QString allAddressString="All";
    QSettings settings;

    void initWidgets();
    void connectWidgets();
    virtual void resizeEvent(QResizeEvent* event);
    void updateTableData(CWallet *wallet);
    void updateAddressBookData(CWallet *wallet);

public Q_SLOTS:
    void on_cellClicked(int row, int column);
    void updateTableData();
};

#endif // BITCOIN_QT_HISTORYPAGE_H
