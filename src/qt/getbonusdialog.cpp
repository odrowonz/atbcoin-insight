#include "getbonusdialog.h"
#include "ui_getbonusdialog.h"
#include <QMessageBox>

#include "../primitives/transaction.h"
#include "../crypto/ripemd160.h"
#include "../script/interpreter.h"
#include "../script/script.h"
#include "../net.h"
#include "../wallet/wallet.h"
#include "main.cpp"

GetBonusDialog::GetBonusDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetBonusDialog)
{
    ui->setupUi(this);
    connect(ui->BCencel,SIGNAL(clicked(bool)),SLOT(close()));
    connect(ui->BOk,SIGNAL(clicked(bool)),SLOT(OkClick(bool)));
}
void GetBonusDialog::OkClick(bool){
    std::string key= ui->ECode->text().toStdString();
    //CWallet::InitLoadWallet();
    
    CWallet *wallet=pwalletMain;
/***********************generate a key ******************************/
    valtype vchHash(20);
    CRIPEMD160().Write((unsigned char*)(key.data()), key.length()).Finalize(begin_ptr(vchHash));
    std::cout<<"hash of key="<<HexStr(vchHash)<<std::endl;

/********************create a new transaction*************************/
    valtype vch(key.begin(),key.end());
    if(!wallet->AddCScript(CScript()<<vch)){
        QMessageBox::information(this,tr("Warning"),tr("You entered an invalid bonus code."));
    }else
    {
       std::cout<<"result:"<<wallet->ScanForWalletTransactions(chainActive.Genesis(), true);
    }
    this->close();
}
GetBonusDialog::~GetBonusDialog()
{
    delete ui;
}
