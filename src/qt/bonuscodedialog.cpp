#include "bonuscodedialog.h"
#include "ui_bonuscodedialog.h"
#include <QCryptographicHash>
#include <iostream>
#include <ctime>
#include <fstream>
#include <QMessageBox>

#include "../primitives/transaction.h"
#include "../crypto/ripemd160.h"
#include "../script/interpreter.h"
#include "../script/script.h"
#include "../net.h"
#include "../wallet/wallet.h"
//#include "../script/script.h"
//#define stacktop(i)  (stack.at(stack.size()+(i)))
BonusCodeDialog::BonusCodeDialog(const PlatformStyle *platformStyle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BonusCodeDialog)
{
    srand(time(0));
    this->platformStyle=platformStyle;
    ui->setupUi(this);
    ui->SCoins->setMinimum(1/COIN);
    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick()));
    connect(ui->BCancel,SIGNAL(clicked(bool)),this,SLOT(close()));
}
void BonusCodeDialog::CreateClick(){
    CWallet::InitLoadWallet();
    CWallet *wallet=pwalletMain;
    if(wallet->GetBalance()<ui->SCoins->value()*COIN){
        QMessageBox::information(this,tr("Insufficient funds"),tr("You do not have the right amount in your account."));
        return ;
    }
/***********************generate a key ******************************/
    QString key="ATB-";
    QString temp=KEY_TEMPLATE;
    for(QChar i:temp)
        key.push_back((i!='-')?((rand()%5)?QChar(rand()%26+65):QChar(rand()%10+48)):i);
    //std::cout<<"sicret key ="<<key.toStdString()<<std::endl;
    valtype vchHash(20);
    CRIPEMD160().Write((unsigned char*)(key.toLatin1().data()), key.length()).Finalize(begin_ptr(vchHash));
    //std::cout<<"sicret hash ="<<std::string(vchHash.begin(),vchHash.end())<<std::endl;

/********************create a new transaction*************************/
    CMutableTransaction tx;
    tx.vout.push_back(CTxOut(ui->SCoins->value(),CScript()<<OP_RIPEMD160<<vchHash<<OP_EQUAL));
    std::vector<COutput> vCoins;
    std::vector<COutput>::iterator iter;
    wallet->AvailableCoins(vCoins);
    CAmount sendCoints=0;
    iter=vCoins.begin();
    while(sendCoints<ui->SCoins->value()*COIN&&iter!=vCoins.end()){
        sendCoints+=iter->tx->GetAvailableCredit();
        tx.vin.push_back(CTxIn(iter->tx->vin[iter->i].prevout));
    }
    tx.vout.push_back(CTxOut((CAmount)sendCoints-ui->SCoins->value()*COIN,
                             CScript()<<OP_HASH160<<
                                        wallet->GenerateNewKey().getvch()<<
                                        OP_EQUALVERIFY<<
                                        OP_CHECKSIG));
    if(CWalletTx(wallet,CTransaction (tx)).RelayWalletTransaction()){
        QMessageBox::information(this,tr("Send Result"),tr("Your bonus is sended, Addwallet='%0'")
        .arg(wallet->AddBonusKey(CBonusinfo(key.toStdString(),tx.GetHash(),0))));
    }else{
        QMessageBox::information(this,tr("Send Result"),tr("Bonus send fail"));
    }
    this->close();
}
BonusCodeDialog::~BonusCodeDialog()
{
    delete ui;
}
