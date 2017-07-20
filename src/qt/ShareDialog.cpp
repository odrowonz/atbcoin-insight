#include "ShareDialog.h"
#include "ui_ShareDialog.h"
#include "../main.h"
#include "walletmodel.h"
#include "../wallet/wallet.h"
#include "clientmodel.h"
#include "../pubkey.h"
#include "../txmempool.h"
#include "informationdialog.h"
ShareDialog::ShareDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShareDialog)
{
    ui->setupUi(this);    this->setWindowFlags(this->windowFlags()& ~Qt::WindowContextHelpButtonHint);

    connect(ui->cancelButtonByEmail,SIGNAL(clicked(bool)),SLOT(close()));
    connect(ui->cancelButtonByKey,SIGNAL(clicked(bool)),SLOT(close()));
    connect(ui->sendButtonByEmail,SIGNAL(clicked(bool)),this,SLOT(sendClickedByEmail()));
    connect(ui->sendButtonByKey,SIGNAL(clicked(bool)),this,SLOT(sendClickedByKey()));
    ui->ResponceByEmail->setVisible(false);
    ui->ResponceByKey->setVisible(false);
}
void ShareDialog::resizeEvent(QResizeEvent *){
    ui->tab->setStyleSheet(QString("QTabBar::tab {width:%0;}").arg(this->width()/2.1));
}
void ShareDialog::sendClickedByKey(){
    ui->ResponceByKey->setVisible(true);
    std::string key= ui->KeyEdit->text().toStdString();
    key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
    CPubKey pubkey;
    pwalletMain->GetKeyFromPool(pubkey);
    if(!pwalletMain->AddCryptedKey(pubkey,std::vector<unsigned char>(key.begin(),key.end()))){
        ui->ResponceByKey->setText(tr("This key is not valid."));
        return;
    }
    CWallet* wallet =pwalletMain;
    CAmount ammout=wallet->GetBalance();
    pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
    pwalletMain->ReacceptWalletTransactions();
    if(ammout>=pwalletMain->GetBalance()){
        ui->ResponceByKey->setText(tr("Could not get share by key (%0)").arg(QString::fromStdString(key)));
        return;
    }
    ui->ResponceByKey->setText(tr("Your share is added to the wallet."));
}
void ShareDialog::sendClickedByEmail(){
    ui->ResponceByEmail->setVisible(true);
    ui->ResponceByEmail->setText("Response");
}
ShareDialog::~ShareDialog()
{
    delete ui;
}
