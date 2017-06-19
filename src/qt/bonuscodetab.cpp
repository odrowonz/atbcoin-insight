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
BonusCodeTab::BonusCodeTab(WalletModel *wmodel_, const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BonusCodeTab)
{
    wmodel=wmodel_;
    this->platformStyle=platformStyle;
    ui->setupUi(this);
    ui->CouponList->setModel(model=new QSortFilterProxyModel(this));
    ui->CouponList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStandardItemModel *couponModel=new QStandardItemModel;
    couponModel->setHorizontalHeaderLabels(QStringList()<<tr("Date")<<tr("nVout")<<tr("Amount")<<tr("Transaction hash")<<tr("KeyWord")<<tr("status"));
    model->setSourceModel(couponModel);
    model->setDynamicSortFilter(true);
    model->setSortCaseSensitivity(Qt::CaseInsensitive);
    model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    model->setSortRole(Qt::EditRole);
    ui->CouponList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->CouponList->setAlternatingRowColors(true);
    ui->CouponList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->CouponList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->CouponList->setSortingEnabled(true);
    ui->CouponList->sortByColumn(0, Qt::DescendingOrder);
    ui->CouponList->verticalHeader()->hide();
    ui->CouponList->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    ui->CouponList->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    ui->CouponList->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    ui->CouponList->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->CouponList->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Stretch);
    ui->CouponList->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Fixed);
    ui->CouponList->setColumnWidth(0,110);
    ui->CouponList->setColumnWidth(1,60);
    ui->CouponList->setColumnWidth(2,120);
    ui->CouponList->setColumnWidth(5,80);
    ui->tab1->setCurrentIndex(1);
    ui->SAmount->setMinimum(0.001);
    ui->SAmount->setMaximum(999999999*CUSTOM_FACTOR);
    ui->SAmount->setDecimals(3);
    ui->SAmount->setSingleStep(0.001);
    ui->BCreate->setIcon(platformStyle->SingleColorIcon(":/icons/c_coupon"));
    ui->BReceive->setIcon(platformStyle->SingleColorIcon(":/icons/r_coupon"));
    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick(bool)));
    connect(ui->BReceive,SIGNAL(clicked(bool)),this,SLOT(getBonusClick(bool)));
    connect(ui->tab1,SIGNAL(currentChanged(int)),this,SLOT(updateBonusList()));
    connect(ui->CouponList,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(cliced(QModelIndex)));
}
bool BonusCodeTab::keyCheck(const std::string &str){
    std::string base(KEY_TEMPLATE);
    return str.substr(0,4)=="ATB-"&&str.size()-4==base.size()&&
            (str[12]=='-'||str[21]=='-'||str[30]=='-'||str[39]=='-');
}
void BonusCodeTab::cliced(QModelIndex i){
    TransactionDescDialog(*model,i.row(),this).exec();
}
void BonusCodeTab::updateBonusList(){
    model->removeRows(0,model->rowCount());
    for(Bonusinfoset::iterator i=pwalletMain->GetListOfBonusCodes().begin();i!=pwalletMain->GetListOfBonusCodes().end();i++){
        CTransaction tx;
        uint256 hashBlock;
        const CCoins* coins = pcoinsTip->AccessCoins(i->hashTx);
        if(coins!=NULL&&GetTransaction(i->hashTx, tx, Params().GetConsensus(), hashBlock, true)){
            model->insertRow(0);

            if(coins->IsAvailable(i->nVout)){
                model->setData(model->index(0,5), QIcon(":/icons/unused"), Qt::DecorationRole);
                model->setData(model->index(0,5),tr("Unused"),Qt::DisplayRole);
            }else{
                model->setData(model->index(0,5), QIcon(":/icons/used"), Qt::DecorationRole);
                model->setData(model->index(0,5),tr("Used"),Qt::DisplayRole);
            }
            model->setData(model->index(0,4),QString::fromStdString(i->key));
            model->setData(model->index(0,3),QString::fromStdString(i->hashTx.ToString()));
            model->setData(model->index(0,2),QString::number(tx.vout[i->nVout].nValue/(double)CUSTOM_FACTOR,'f'));
            model->setData(model->index(0,1),i->nVout);
            model->setData(model->index(0,0),QDateTime::fromTime_t(tx.nTime).toString("M.d.yyyy HH:mm"));
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
    if(!pwalletMain->AddCScript(s)){
        InformationDialog msgBox(tr("This key is added into your wallet."),"","",this);
        msgBox.exec();
        return;
    }
    CAmount ammout=pwalletMain->GetBalance();
    pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
    pwalletMain->ReacceptWalletTransactions();
    wmodel->updateTransaction();
    wmodel->pollBalanceChanged();
    if(ammout>=pwalletMain->GetBalance()){
        InformationDialog msgBox(tr("This key is no longer valid."),"","",this);
        msgBox.exec();
        return;
    }
    std::map<uint256, CWalletTx>::iterator i=pwalletMain->mapWallet.begin();
    while(i!=pwalletMain->mapWallet.end()){
        if(i->second.IsTrusted()){
            for(CTxOut vout:i->second.vout){
                uint160 temp3= Hash160(CScript()<<valtype(key.begin(),key.end()));
                valtype temp4(temp3.begin(),temp3.end());
                if(vout.scriptPubKey==CScript()<<OP_0<<OP_DROP<<OP_HASH160<<temp4<<OP_EQUAL){
                    InformationDialog msgBox(tr("ATB coins were received with this coupon"),QString::number((double)vout.nValue/COIN,'f'),QString::fromStdString(key),this);
                    msgBox.exec();
                   Q_EMIT couponAdded(QString::fromStdString(i->first.ToString()));
                }
            }
        }
        i++;
    }
    ui->EKey->clear();
}
void BonusCodeTab::CreateClick(bool){
    CWallet *wallet=pwalletMain;
    if(wallet->GetBalance()<=ui->SAmount->value()*CUSTOM_FACTOR){
        InformationDialog msgBox(tr("You do not have the right amount in your account."),"","",this);
        msgBox.exec();
        return ;
    }
/***********************generate a key ******************************/

    std::string key="ATB-";
    unsigned int Entropy_source=0x0;
    while (!Entropy_source){
        void * temp =malloc(0x1);
        Entropy_source= size_t(temp);
        free(temp);
    }
    srand(Entropy_source);
    std::string temp=KEY_TEMPLATE;
    for(unsigned char i:temp)
        key.push_back((i!='-')?((rand()%5)?char(rand()%26+65):char(rand()%10+48)):i);
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
        InformationDialog(tr("Your coupon is created. The coupon will be available after it is added to the block."),"","",this).exec();
        int i=0;while(wtx.vout.size()!=i&&wtx.vout[i].scriptPubKey!=rec.scriptPubKey)++i;
        if(i==wtx.vout.size()){
            InformationDialog(tr("coupon send fail"),"","",this).exec();
            return;
        }
        wallet->AddBonusKey(CBonusinfo(key,wtx.GetHash(),i));
    }else{
        InformationDialog(tr("coupon send fail"),"","",this).exec();
    }
    updateBonusList();
}
BonusCodeTab::~BonusCodeTab()
{
    delete ui;
}