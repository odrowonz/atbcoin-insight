#include "bonuscodetab.h"
#include "ui_bonuscodetab.h"
#include "walletmodel.h"
#include "../wallet/wallet.h"
#include <QTime>
#include "../main.h"
#include <QMessageBox>
BonusCodeTab::BonusCodeTab(WalletModel *wmodel_, const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BonusCodeTab)
{
    wmodel=wmodel_;
    this->platformStyle=platformStyle;
    ui->setupUi(this);
    ui->CouponList->setModel(model=new QStandardItemModel(0,3));
    ui->CouponList->setEditTriggers(QAbstractItemView::DoubleClicked);

    ui->SAmount->setMinimum(1/COIN);
    ui->SAmount->setMaximum(999999999*COIN);
    ui->SAmount->setDecimals(8);
    ui->SAmount->setSingleStep(1.0/COIN);

    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick(bool)));
    connect(ui->BReceive,SIGNAL(clicked(bool)),this,SLOT(getBonusClick(bool)));
    connect(ui->BClearAmount,SIGNAL(clicked(bool)),ui->SAmount,SLOT(clear()));
    connect(ui->BClearKey,SIGNAL(clicked(bool)),ui->EKey,SLOT(clear()));
    updateBonusList();
}
void BonusCodeTab::updateBonusList(){
    model->clear();
    model->setHorizontalHeaderLabels(QStringList()<<tr("time")<<tr("nVout")<<tr("Amount")<<tr("Transaction hash")<<tr("KeyWord"));
    ui->CouponList->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    ui->CouponList->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    ui->CouponList->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    ui->CouponList->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->CouponList->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Stretch);
    ui->CouponList->setColumnWidth(0,160);
    ui->CouponList->setColumnWidth(1,60);
    ui->CouponList->setColumnWidth(2,120);
    for(Bonusinfoset::iterator i=pwalletMain->GetListOfBonusCodes().begin();i!=pwalletMain->GetListOfBonusCodes().end();i++){
        CTransaction tx;
        uint256 hashBlock;
        if(GetTransaction(i->hashTx, tx, Params().GetConsensus(), hashBlock, true)){
            model->insertRow(0);
            model->setData(model->index(0,4),QString::fromStdString(i->key));
            model->setData(model->index(0,3),QString::fromStdString(i->hashTx.ToString()));
            model->setData(model->index(0,2),(tx.vout[i->nVout].nValue/(double)COIN));
            model->setData(model->index(0,1),i->nVout);
            model->setData(model->index(0,0),QDateTime::fromTime_t(tx.nTime).toString());
        }
    }
}
void BonusCodeTab::setWalletModel(WalletModel *wmodel){
    this->wmodel=wmodel;
}
void BonusCodeTab::getBonusClick(bool){
    //(new GetBonusDialog(this))->exec();
    std::string key= ui->EKey->text().toStdString();
/********************create a new transaction*************************/
    valtype vch(key.begin(),key.end());
    CScript s= CScript()<<vch;
    pwalletMain->AddCScript(s);
    pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
    pwalletMain->ReacceptWalletTransactions();
    wmodel->updateTransaction();
    wmodel->pollBalanceChanged();
}
void BonusCodeTab::CreateClick(bool){
    CWallet *wallet=pwalletMain;
    if(wallet->GetBalance()<ui->SAmount->value()*COIN){
        QMessageBox::information(this,tr("Insufficient funds"),tr("You do not have the right amount in your account."));
        return ;
    }
/***********************generate a key ******************************/

    std::string key="ATB-";
    std::string temp=KEY_TEMPLATE;
    for(unsigned char i:temp)
        key.push_back((i!='-')?((rand()%5)?char(rand()%26+65):char(rand()%10+48)):i);
    uint160 temp3= Hash160(CScript()<<valtype(key.begin(),key.end()));
    valtype temp4(temp3.begin(),temp3.end());


/********************create a new transaction*************************/
    std::vector<CRecipient> Recipient;
    CRecipient rec;
    rec.scriptPubKey=CScript()<<OP_HASH160<<temp4<<OP_EQUAL;
    rec.nAmount=round(ui->SAmount->value()*COIN);
    rec.fSubtractFeeFromAmount=false;
    Recipient.push_back(rec);
    CWalletTx wtx;
    CReserveKey Rkey(wallet);
    std::string fall;
    CAmount nFeeRet=1;
    int nChangePosInOut=0;
    wallet->CreateTransaction(Recipient,wtx,Rkey,nFeeRet,nChangePosInOut,fall);
    if(wallet->CommitTransaction(wtx,Rkey)){
        QMessageBox::information(this,tr("Send Result"),tr("Your bonus is sended. The bonus will be available after it is added to the block."));
        int i=0;while(wtx.vout.size()!=i&&wtx.vout[i].scriptPubKey!=rec.scriptPubKey)++i;
        if(i==wtx.vout.size()){
            QMessageBox::information(this,tr("Send Result"),tr("Bonus send fail"));
        }
        wallet->AddBonusKey(CBonusinfo(key,wtx.GetHash(),i));
    }else{
        QMessageBox::information(this,tr("Send Result"),tr("Bonus send fail"));
    }
    updateBonusList();
}
BonusCodeTab::~BonusCodeTab()
{
    delete ui;
}
