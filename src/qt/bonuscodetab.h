#ifndef BONUSCODETAB_H
#define BONUSCODETAB_H

#include "platformstyle.h"
#include <QWidget>
#include "walletmodel.h"
#include "clientmodel.h"
#include "../txmempool.h"
#include "transactionfilterproxy.h"
#include <QStandardItemModel>
#define KEY_TEMPLATE "3EIOPJ4A-JMWUIGPV-NR76TESB-UYWH1UJS-HWJOXSWK"
namespace Ui {
class BonusCodeTab;

}

class BonusCodeTab : public QWidget
{
    Q_OBJECT

public:
    explicit BonusCodeTab (WalletModel *wmodel, const PlatformStyle *platformStyle, QWidget *parent = 0);
    void setWalletModel(WalletModel *wmodel);
    void setClientModel(ClientModel *clientModel);
    ~BonusCodeTab();
public Q_SLOTS:
    void updateBonusList();
private:
    Ui::BonusCodeTab *ui;
    ClientModel *clientModel;
    WalletModel *wmodel;
    const PlatformStyle *platformStyle;
    bool keyCheck(const std::string &str);
    CWalletTx* findTx(const CScript& script);
    QSortFilterProxyModel *model;
private Q_SLOTS:
    void cliced(QModelIndex);
    void getBonusClick(bool);
    void CreateClick(bool);
Q_SIGNALS:
    void couponAdded(const QString&);// emitted when a new coupon added into wallet. where a parameter it is hash of transaction with coupon
};

#endif // BONUSCODETAB_H
