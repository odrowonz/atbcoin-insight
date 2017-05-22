#include "bonuscodedialog.h"
#include "ui_bonuscodedialog.h"
#include <QCryptographicHash>
#include <ctime>
#include <QMessageBox>

#include "../primitives/transaction.h"
#include "../crypto/ripemd160.h"
#include "../script/interpreter.h"
#include "../script/script.h"
#include "../net.h"
#include "../wallet/wallet.h"
BonusCodeDialog::BonusCodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BonusCodeDialog)
{
    srand(time(0));
    ui->setupUi(this);
    ui->SCoins->setMinimum(1/COIN);
    ui->SCoins->setMaximum(999999999);
    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick()));
    connect(ui->BCancel,SIGNAL(clicked(bool)),this,SLOT(close()));
}
void BonusCodeDialog::CreateClick(){
    CWallet *wallet=pwalletMain;
    if(wallet->GetBalance()<ui->SCoins->value()*COIN){
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
    rec.nAmount=ui->SCoins->value()*COIN;
    rec.fSubtractFeeFromAmount=false;
    Recipient.push_back(rec);
    CWalletTx wtx;
    CReserveKey Rkey(wallet);
    std::string fall;
    CAmount nFeeRet=1000;
    int nChangePosInOut=0;
    wallet->CreateTransaction(Recipient,wtx,Rkey,nFeeRet,nChangePosInOut,fall);
    if(wallet->CommitTransaction(wtx,Rkey)){
        QMessageBox::information(this,tr("Send Result"),tr("Your bonus is sended"));
        int i=0;while(wtx.vout.size()!=i&&wtx.vout[i].scriptPubKey!=rec.scriptPubKey)++i;
        if(i==wtx.vout.size()){
            QMessageBox::information(this,tr("Send Result"),tr("Bonus send fail"));
        }
        wallet->AddBonusKey(CBonusinfo(key,wtx.GetHash(),i));
    }else{
        QMessageBox::information(this,tr("Send Result"),tr("Bonus send fail"));
    }
    this->close();
}
BonusCodeDialog::~BonusCodeDialog()
{
    delete ui;
}
