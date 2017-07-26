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
    std::string mail_pass=ui->EMailEdit->text().toStdString()+ui->PassEdit->text().toStdString();
    valtype vch(mail_pass.begin(),mail_pass.end());
    CScript s= CScript()<<vch;
    ui->ResponceByEmail->setVisible(true);
    ui->ResponceByEmail->setStyleSheet("QLabel { color: #f6e395; }");
    if(!pwalletMain->AddCScript(s)){
        ui->ResponceByEmail->setText(tr("You have already added information to get a share."));
        ui->PassEdit->clear();
        return;
    }
    CAmount ammout=pwalletMain->GetBalance();
    pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
    pwalletMain->ReacceptWalletTransactions();
    if(ammout>=pwalletMain->GetBalance()){
        ui->ResponceByEmail->setText(tr("The balance for this private key was not found.\n Maybe you entered your email or password incorrectly."));
        ui->PassEdit->clear();
        return;
    }
    ui->ResponceByEmail->setStyleSheet("QLabel { color: #a3f642; }");
    ui->ResponceByEmail->setText("The balance for this private key was not found.");
    InformationDialog msgBox(tr("Your share is %0 coins.").arg(QString::number(pwalletMain->GetBalance()-ammout)));
    msgBox.exec();
}
ShareDialog::~ShareDialog()
{
    delete ui;
}
