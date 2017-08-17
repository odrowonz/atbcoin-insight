#include "bonuscodetab.h"
#include "ui_bonuscodetab.h"
#include "../wallet/wallet.h"
#include <QTime>
#include "../main.h"
#include <QMessageBox>
#include <ctime>
#include "transactiontablemodel.h"
#include "transactiondescdialog.h"
#include "guiconstants.h"
#include "informationdialog.h"
#include "../coincontrol.h"
#include "../script/standard.h"
#include "previewcodedialog.h"
#include "../key.h"

BonusCodeTab::BonusCodeTab(WalletModel *wmodel_, const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BonusCodeTab)
{
    wmodel=wmodel_;
    this->platformStyle=platformStyle;
    ui->setupUi(this);    this->setWindowFlags(this->windowFlags()& ~Qt::WindowContextHelpButtonHint);


    tableInit(ui->CouponList);
    tableInit(ui->usedCouponList);

    ui->tab1->setCurrentIndex(0);

    ui->SAmount->setMinimum(0.001);
    ui->SAmount->setAlignment(Qt::AlignCenter);
    ui->SAmount->setMaximum(999999999*CUSTOM_FACTOR);
    ui->SAmount->setDecimals(3);
    ui->SAmount->setSingleStep(0.001);
    ui->BCreate->setIcon(platformStyle->SingleColorIcon(":/icons/c_coupon",Qt::white));
    ui->BReceive->setIcon(platformStyle->SingleColorIcon(":/icons/r_coupon",Qt::white));
    ui->CouponId->setText(ui->CouponId->text()+":");


    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick(bool)));
    connect(ui->BReceive,SIGNAL(clicked(bool)),this,SLOT(getBonusClick(bool)));
    connect(ui->tab1,SIGNAL(currentChanged(int)),this,SLOT(updateBonusList()));

}
void BonusCodeTab::Clicked(QModelIndex i){
    PreviewCodeDialog(i.model(),i.row(),this).exec();
}
void BonusCodeTab::resizeEvent(QResizeEvent *){
    ui->tab1->setStyleSheet(QString("QTabBar::tab {width:%0;}").arg(this->width()/2.1));
}
void BonusCodeTab::tableInit(QTableView *sourceTable){
    QSortFilterProxyModel * model;
    sourceTable->setModel(model=new QSortFilterProxyModel(this));
    sourceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStandardItemModel *couponModel=new QStandardItemModel;
    couponModel->setHorizontalHeaderLabels(QStringList()<<tr("Date")<<tr("Amount")<<tr("Transaction hash")<<tr("KeyWord")<<tr("status"));
    model->setSourceModel(couponModel);
    model->setDynamicSortFilter(true);
    model->setSortCaseSensitivity(Qt::CaseInsensitive);
    model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    model->setSortRole(Qt::EditRole);
    sourceTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sourceTable->setAlternatingRowColors(true);
    sourceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    sourceTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    sourceTable->setSortingEnabled(true);
    sourceTable->sortByColumn(0, Qt::DescendingOrder);
    sourceTable->verticalHeader()->hide();
    sourceTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    sourceTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    sourceTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    sourceTable->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    sourceTable->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Fixed);
    sourceTable->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter|Qt::AlignVCenter);


    sourceTable->setColumnWidth(0,140);
    sourceTable->setColumnWidth(1,100);
    sourceTable->setColumnHidden(2,true);
    sourceTable->setColumnWidth(4,130);
    sourceTable->setShowGrid(false);

    connect(sourceTable,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(Clicked(QModelIndex)));

}
void BonusCodeTab::updateBonusList(){
    QStandardItemModel *model=static_cast<QStandardItemModel* >((static_cast<QSortFilterProxyModel*>(ui->CouponList->model()))->sourceModel());
    QStandardItemModel *usedmodel=static_cast<QStandardItemModel* >((static_cast<QSortFilterProxyModel*>(ui->usedCouponList->model()))->sourceModel());

    model->removeRows(0,model->rowCount());
    usedmodel->removeRows(0,usedmodel->rowCount());
    for(Bonusinfoset::iterator i=pwalletMain->GetListOfBonusCodes().begin();i!=pwalletMain->GetListOfBonusCodes().end();i++){
        const CCoins* coins = pcoinsTip->AccessCoins(i->hashTx);
        const CWalletTx *tx(pwalletMain->GetWalletTx(i->hashTx));
        if(tx){
            if(i->isUsed()){
                usedmodel->insertRow(0);
                if(coins!=NULL&&coins->IsAvailable(i->getnVout(tx->nTime>1501545600)))
                     usedmodel->setData(usedmodel->index(0,4),tr("Unconfirmed"),Qt::DisplayRole);
                else
                     usedmodel->setData(usedmodel->index(0,4),tr("Used"),Qt::DisplayRole);

                usedmodel->setData(usedmodel->index(0,3),QString::fromStdString(i->key));
                usedmodel->setData(usedmodel->index(0,2),QString::fromStdString(tx->GetHash().ToString()));
                usedmodel->setData(usedmodel->index(0,1),tx->vout[i->getnVout(tx->nTime>1501545600)].nValue/(double)CUSTOM_FACTOR);
                usedmodel->setData(usedmodel->index(0,0),QDateTime::fromTime_t(tx->nTimeReceived));

                usedmodel->setData(usedmodel->index(0,0),Qt::AlignCenter, Qt::TextAlignmentRole);
                usedmodel->setData(usedmodel->index(0,1),Qt::AlignCenter, Qt::TextAlignmentRole);
                usedmodel->setData(usedmodel->index(0,2),Qt::AlignCenter, Qt::TextAlignmentRole);
                usedmodel->setData(usedmodel->index(0,3),Qt::AlignCenter, Qt::TextAlignmentRole);
                usedmodel->setData(usedmodel->index(0,4),Qt::AlignCenter, Qt::TextAlignmentRole);
            }else{
                model->insertRow(0);
                if(coins!=NULL&&coins->IsAvailable(i->getnVout(tx->nTime>1501545600))){
                        model->setData(model->index(0,4),tr("Unused"),Qt::DisplayRole);
                }else{
                    if(mempool.exists(tx->GetHash())){
                        model->setData(model->index(0,4),tr("Unconfirmed"),Qt::DisplayRole);
                    }else{
                        model->setData(model->index(0,4),tr("Used"),Qt::DisplayRole);
                    }
                }
                model->setData(model->index(0,3),QString::fromStdString(i->key));
                model->setData(model->index(0,2),QString::fromStdString(tx->GetHash().ToString()));
                model->setData(model->index(0,1),tx->vout[i->getnVout(tx->nTime>1501545600)].nValue/(double)CUSTOM_FACTOR);
                model->setData(model->index(0,0),QDateTime::fromTime_t(tx->nTime));

                model->setData(model->index(0,0),Qt::AlignCenter, Qt::TextAlignmentRole);
                model->setData(model->index(0,1),Qt::AlignCenter, Qt::TextAlignmentRole);
                model->setData(model->index(0,2),Qt::AlignCenter, Qt::TextAlignmentRole);
                model->setData(model->index(0,3),Qt::AlignCenter, Qt::TextAlignmentRole);
                model->setData(model->index(0,4),Qt::AlignCenter, Qt::TextAlignmentRole);
            }
        }
    }
}
void BonusCodeTab::setWalletModel(WalletModel *wmodel){
    this->wmodel=wmodel;
}

void BonusCodeTab::setClientModel(ClientModel *clientmodel){
    clientModel=clientmodel;
    connect(clientModel,SIGNAL(numBlocksChanged(int,QDateTime,double,bool)),this,SLOT(updateBonusList()));
}

void BonusCodeTab::getBonusClick(bool){
    std::string key_text= ui->EKey->text().toStdString();
    key_text.erase(std::remove(key_text.begin(), key_text.end(), ' '), key_text.end());
    key_text.erase(0,4);
    key_text.erase(std::remove(key_text.begin(),key_text.end(),'-'), key_text.end());
    CBitcoinSecret vchSecret;
    bool fGood = vchSecret.SetString(key_text);
    if(!fGood){
        InformationDialog msgBox(tr("Invalid key: Check the key and try again."),"","",this);
        msgBox.exec();
        ui->EKey->clear();
        return;
    }
    CKey key = vchSecret.GetKey();
    if (!key.IsValid()){
        InformationDialog(tr("Private key outside allowed range."),"","",this).exec();
        return;
    }
    CPubKey pubkey = key.GetPubKey();
    WalletModel::UnlockContext ctx(wmodel->requestUnlock());
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        return;
    }
    pwalletMain->nTimeFirstKey = 1; // 0 would be considered 'no value'
    if (!pwalletMain->AddKeyPubKey(key, pubkey)){
        InformationDialog(tr("Error adding key to wallet."),"","",this).exec();
        return;
    }
    pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
    std::map<uint256, CWalletTx>::iterator i=pwalletMain->mapWallet.begin();
    while(i!=pwalletMain->mapWallet.end()){
        if(i->second.IsTrusted()&&i->second.GetAvailableCredit()>0){
            int nvout=0;
            for(CTxOut vout:i->second.vout){
                if(vout.scriptPubKey==GetScriptForRawPubKey(pubkey)){
                    InformationDialog msgBox(tr("%0 ATBcoins were received with this code"),
                                             QString::number((double)vout.nValue/COIN,'f'),
                                             QString::fromStdString(ui->EKey->text().toStdString()),
                                             this);
                    msgBox.exec();
                    confirmation(CBonusinfo(pubkey.GetHash().ToString(),i->first,nvout,true));
                }
                nvout++;
            }
        }
        i++;
    }
    updateBonusList();
    ui->EKey->clear();
}
void BonusCodeTab::CreateClick(bool){
    CWallet *wallet=pwalletMain;
    if(wallet->GetBalance()<=ui->SAmount->value()*CUSTOM_FACTOR){
        InformationDialog msgBox(tr("You do not have the right amount in your account."),"","",this);
        msgBox.exec();
        return ;
    }

    WalletModel::UnlockContext ctx(wmodel->requestUnlock());
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        return;
    }

/***********************generate a key ******************************/

    std::string showkey="ATB";
    CKey Key;
    Key.MakeNewKey(false);
    if(!Key.IsValid()){
        InformationDialog(tr("Key create fail"),"","",this).exec();
        return;
    }
    std::string temp=CBitcoinSecret(Key).ToString();
    for(unsigned int i=0;i<temp.size();i+=10){
        temp.insert(i,"-");
    }
    showkey+=temp;
/********************create a new transaction*************************/
    std::vector<CRecipient> Recipient;
    CRecipient rec;
    rec.scriptPubKey=GetScriptForRawPubKey(Key.GetPubKey());
    rec.nAmount=round(ui->SAmount->value()*CUSTOM_FACTOR);
    rec.fSubtractFeeFromAmount=false;
    Recipient.push_back(rec);
    CWalletTx wtx;
    CReserveKey Rkey(wallet);
    std::string fall;
    CAmount nFeeRet=1;
    int nChangePosInOut=0;
    if(wallet->CreateTransaction(Recipient,wtx,Rkey,nFeeRet,nChangePosInOut,fall)&&wallet->CommitTransaction(wtx,Rkey)){
        int i=0;while(wtx.vout.size()!=i&&wtx.vout[i].scriptPubKey!=rec.scriptPubKey)++i;
        if(i==wtx.vout.size()){
            InformationDialog(tr("Code create fail"),"","",this).exec();
            return;
        }
        wallet->AddBonusKey(CBonusinfo(showkey,wtx.GetHash(),i));
        updateBonusList();
        InformationDialog(tr("Your code is created. The code will be available after confirmation."),
                          QString::number(ui->SAmount->value(),'f'),QString::fromStdString(showkey),this).exec();

    }else{
        InformationDialog(tr("Code create fail"),"","",this).exec();
    }
}


void BonusCodeTab::confirmation(const CBonusinfo& info){
    std::vector<CRecipient> Recipient;
    CRecipient rec;
    CPubKey key;
    CCoinControl control;
    control.Select(COutPoint(info.hashTx,info.getnVout()));
    pwalletMain->GetKeyFromPool(key);
    CKeyID keyID = key.GetID();
    CBitcoinAddress address(keyID);
    rec.scriptPubKey=GetScriptForDestination(address.Get());
    rec.nAmount=COIN*0.0001;
    rec.fSubtractFeeFromAmount=false;
    Recipient.push_back(rec);
    CWalletTx wtx;
    CReserveKey Rkey(pwalletMain);
    std::string fall;
    CAmount nFeeRet=1;
    int nChangePosInOut=0;
    wtx.mapValue["comment"] = tr("Commission for the confirmation of the bonus code.").toStdString();
    wtx.mapValue["bonusConfirmation"] = info.key;
    if(!(pwalletMain->CreateTransaction(Recipient,wtx,Rkey,nFeeRet,nChangePosInOut,fall,&control)&&pwalletMain->CommitTransaction(wtx,Rkey))){
        InformationDialog(tr("The key is no confirmed."),"","",this).exec();
        return;
    }
    pwalletMain->AddBonusKey(info);
}


BonusCodeTab::~BonusCodeTab()
{
    delete ui;
}
