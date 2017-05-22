#include "getbonusdialog.h"
#include "ui_getbonusdialog.h"
#include <QMessageBox>

#include "../primitives/transaction.h"
#include "../crypto/ripemd160.h"
#include "../script/interpreter.h"
#include "../script/script.h"
#include "../net.h"
#include "../utilstrencodings.cpp"
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
    //std::string key="0";
    //CWallet::InitLoadWallet();    
/***********************generate a key ******************************/
   // valtype vchHash(20);
    //CRIPEMD160().Write((unsigned char*)(key.data()), key.length()).Finalize(begin_ptr(vchHash));
    
    std::cout<<"(generated key)="<<key<<std::endl;
   // std::cout<<"hash of key="<<HexStr(vchHash)<<std::endl;
    //valtype vchHash2(20);
    //std::cout<<"До="<<pwalletMain->GetBalance()<<std::endl;
/********************create a new transaction*************************/
    valtype vch(key.begin(),key.end());
    uint160 temp= Hash160(valtype(key.begin(),key.end()));
   // valtype vch();
   // CScript s= CScript(vch.begin(),vch.end());
    CScript s= CScript()<<vch;
    pwalletMain->AddCScript(s);
    std::cout<<"hash of add script (HexStr(Hash160(s))) ="<<HexStr(Hash160(s))<<std::endl<<
    "hash of add script (CScriptID(Hash160(s)).GetHex()) ="<<CScriptID(Hash160(s)).GetHex()<<std::endl;
    pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
    pwalletMain->ReacceptWalletTransactions();
   /* if(!){
        QMessageBox::information(this,tr("Warning"),tr("You entered an invalid bonus code."));
    }else
    {
    }*/
    //std::cout<<"После="<<pwalletMain->GetBalance()<<std::endl;
    this->close();
}
GetBonusDialog::~GetBonusDialog()
{
    delete ui;
}
