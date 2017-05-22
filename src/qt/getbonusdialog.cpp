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
/********************create a new transaction*************************/
    valtype vch(key.begin(),key.end());
    CScript s= CScript()<<vch;
    pwalletMain->AddCScript(s);
    pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
    pwalletMain->ReacceptWalletTransactions();
    this->close();
}
GetBonusDialog::~GetBonusDialog()
{
    delete ui;
}
