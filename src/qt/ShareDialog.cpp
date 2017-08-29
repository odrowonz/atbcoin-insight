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
#include <QFile>
#include <QTextStream>
ShareDialog::ShareDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShareDialog)
{

    ui->setupUi(this);    this->setWindowFlags(this->windowFlags()& ~Qt::WindowContextHelpButtonHint);
    this->setModal(true);
    connect(ui->cancelButtonByKey,SIGNAL(clicked(bool)),SLOT(close()));
    connect(ui->sendButtonByKey,SIGNAL(clicked(bool)),this,SLOT(sendClickedByKey()));
    ui->sendButtonByKey->setDefault(true);
    ui->ResponceByKey->setVisible(false);
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
ShareDialog::~ShareDialog()
{
    delete ui;
}
