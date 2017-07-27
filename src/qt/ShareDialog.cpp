#include "ShareDialog.h"
#include "ui_ShareDialog.h"
#include "../main.h"
#include "walletmodel.h"
#include "../wallet/wallet.h"
#include "clientmodel.h"
#include "../pubkey.h"
#include "../txmempool.h"
#include "informationdialog.h"
#include "../key.h"
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
    std::string key_text= ui->KeyEdit->text().toStdString();
    key_text.erase(std::remove(key_text.begin(), key_text.end(), ' '), key_text.end());
    CBitcoinSecret vchSecret;
    bool fGood = vchSecret.SetString(key_text);
    ui->ResponceByKey->setStyleSheet("QLabel { color: #f6e395; }");
    if (!fGood){
        ui->ResponceByKey->setText(tr("Invalid private key encoding."));
        return;
    }

    CKey key = vchSecret.GetKey();
    if (!key.IsValid()){
        ui->ResponceByKey->setText(tr("Private key outside allowed range."));
        return;
    }
    CPubKey pubkey = key.GetPubKey();
//    std::vector<unsigned char> data(pubkey.g.begin(), pubkey.end());
//    std::stringstream hex;
//    for(std::vector<unsigned char>::iterator i=data.begin(); i!=data.end(); i++){
//        hex << std::hex << (int)*i;
//    }
    InformationDialog(QString::fromStdString(pubkey.GetHash().ToString()),"","",this).exec();
    CKeyID vchAddress = pubkey.GetID();
    {
        pwalletMain->MarkDirty();
        pwalletMain->SetAddressBook(vchAddress, key_text, "receive");
        if (pwalletMain->HaveKey(vchAddress)){
            ui->ResponceByKey->setText(tr("A private key has already been added to your wallet."));
            return;
        }
        pwalletMain->mapKeyMetadata[vchAddress].nCreateTime = 1;

        if (!pwalletMain->AddKeyPubKey(key, pubkey)){
            ui->ResponceByKey->setText(tr("Error adding key to wallet."));
            return;
        }
        pwalletMain->nTimeFirstKey = 1; // 0 would be considered 'no value'
        CAmount ammout=pwalletMain->GetBalance();
        pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
        if(ammout>=(pwalletMain->GetBalance()+pwalletMain->GetImmatureBalance()+pwalletMain->GetUnconfirmedBalance())){
            ui->ResponceByKey->setText(tr("Could not get share by key (%0)").arg(QString::fromStdString(key_text)));
            return;
        }
    }
    ui->ResponceByKey->setStyleSheet("QLabel { color: #a3f642; }");
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
