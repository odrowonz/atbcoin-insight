// Copyright (c) 2011-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transactiondescdialog.h"
#include "ui_transactiondescdialog.h"

#include "transactiontablemodel.h"

#include <QModelIndex>

TransactionDescDialog::TransactionDescDialog(const QModelIndex &idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransactionDescDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Details for %1").arg(idx.data(TransactionTableModel::TxIDRole).toString()));
    QString desc = idx.data(TransactionTableModel::LongDescriptionRole).toString();
    ui->detailText->setHtml(desc);
}
TransactionDescDialog::TransactionDescDialog(const QSortFilterProxyModel &idx, int row, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransactionDescDialog)
{
    ui->setupUi(this);
    setMinimumWidth(750);
    setWindowTitle(tr("Details for coupon"));
    ui->detailText->setHtml(QString (tr("<i><font color='#96946d'>  Key of coupon:</i> %0 <br>"
                                        "<i><font color='#96946d'>  Amount:</i> %1 <br>"
                                        "<i><font color='#96946d'>  Hash of transasction:</i> %2 <br>"
                                        "<i><font color='#96946d'>  Time of creation:</i> %3 <br>")).
                                    arg(idx.data(idx.index(row,3)).toString()).
                                    arg(idx.data(idx.index(row,1)).toString()).
                                    arg(idx.data(idx.index(row,2)).toString()).
                                    arg(idx.data(idx.index(row,0)).toString()));
}
TransactionDescDialog::~TransactionDescDialog()
{
    delete ui;
}
