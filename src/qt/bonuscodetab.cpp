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
#include "../random.h"
// If the bonus code was added to the purse and at the time of the addition it was not valid,
// then the hash of the transaction contains the hash of the script in it, and the viot number is V_OUT_FAIL.
// This is necessary in order that later, when the key is confirmed,
// it was possible to confirm the received money.
#define V_OUT_FAIL 1000;
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
    seed_insecure_rand();


    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick(bool)));
    connect(ui->BReceive,SIGNAL(clicked(bool)),this,SLOT(getBonusClick(bool)));
    connect(ui->tab1,SIGNAL(currentChanged(int)),this,SLOT(updateBonusList()));

}
bool BonusCodeTab::keyCheck(const std::string &str){
    std::string base(KEY_TEMPLATE);
    return str.substr(0,4)=="ATB-"&&str.size()==base.size()&&
            (str[12]=='-'||str[21]=='-'||str[30]=='-'||str[39]=='-');
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
    std::string key= ui->EKey->text().toStdString();
    key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
    if(!keyCheck(key)){
        InformationDialog msgBox(tr("Invalid key: Check the key and try again."),"","",this);
        msgBox.exec();
        ui->EKey->clear();
        return;
    }
    valtype vch(key.begin(),key.end());
    CScript s= CScript()<<vch;
    if(!pwalletMain->AddCBonusScript(s)){
        InformationDialog msgBox(tr("This key is no longer valid."),"","",this);
        msgBox.exec();
        return;
    }
    std::map<uint256, CWalletTx>::iterator i=pwalletMain->mapWallet.begin();
    while(i!=pwalletMain->mapWallet.end()){
        if(i->second.IsTrusted()){
            int nvout=0;
            for(CTxOut vout:i->second.vout){
                uint160 temp3= Hash160(CScript()<<valtype(key.begin(),key.end()));
                valtype temp4(temp3.begin(),temp3.end());
                if(vout.scriptPubKey==CScript()<<OP_0<<OP_DROP<<OP_HASH160<<temp4<<OP_EQUAL){
                    InformationDialog msgBox(tr("%0 ATBcoins were received with this code"),QString::number((double)vout.nValue/COIN,'f'),QString::fromStdString(key),this);
                    msgBox.exec();
                    Q_EMIT couponAdded(QString::fromStdString(i->first.ToString()));
                    confirmation(i->first,nvout,key);
                    CBonusinfo bonus(key,i->first,nvout,true);
                    pwalletMain->AddBonusKey(bonus);
                    updateBonusList();
                }
                nvout++;
            }
        }
        i++;
    }
    ui->EKey->clear();
}
void BonusCodeTab::confirmation(const uint256 &hash, int i,std::string k){
    std::vector<CRecipient> Recipient;
    CRecipient rec;
    CPubKey key;
    CCoinControl control;
    control.Select(COutPoint(hash,i));
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
    wtx.mapValue["bonusConfirmation"] = k;
    if(!(pwalletMain->CreateTransaction(Recipient,wtx,Rkey,nFeeRet,nChangePosInOut,fall,&control)&&pwalletMain->CommitTransaction(wtx,Rkey))){
        InformationDialog(tr("The key is no confirmed."),"","",this).exec();
    }
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

    std::string key;
    std::string temp=KEY_TEMPLATE;
    for(unsigned char i:temp){
        key.push_back((i=='0')?((GetRand(5))?char(GetRand(26)+65):char(GetRand(10)+48)):i);
        srand(rand());
    }
    uint160 temp3= Hash160(CScript()<<valtype(key.begin(),key.end()));
    valtype temp4(temp3.begin(),temp3.end());


/********************create a new transaction*************************/
    std::vector<CRecipient> Recipient;
    CRecipient rec;
    rec.scriptPubKey=CScript()<<OP_0<<OP_DROP<<OP_HASH160<<temp4<<OP_EQUAL;
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
        wallet->AddBonusKey(CBonusinfo(key,wtx.GetHash(),i));
        updateBonusList();
        InformationDialog(tr("Your code is created. The code will be available after confirmation."),
                          QString::number(ui->SAmount->value(),'f'),QString::fromStdString(key),this).exec();

    }else{
        InformationDialog(tr("Code create fail"),"","",this).exec();
    }
}
BonusCodeTab::~BonusCodeTab()
{
    delete ui;
}
