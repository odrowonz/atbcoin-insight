#include "ShareDialog.h"
#include "ui_ShareDialog.h"

ShareDialog::ShareDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShareDialog)
{
    ui->setupUi(this);
    connect(ui->cancelButtonByEmail,SIGNAL(clicked(bool)),SLOT(close()));
    connect(ui->cancelButtonByKey,SIGNAL(clicked(bool)),SLOT(close()));
    connect(ui->sendButtonByEmail,SIGNAL(clicked(bool)),this,SLOT(sendClickedByEmail()));
    connect(ui->sendButtonByKey,SIGNAL(clicked(bool)),this,SLOT(sendClickedByKey()));
    //ui->ResponceByEmail->hide();
    ui->ResponceByEmail->setVisible(false);
    ui->ResponceByKey->setVisible(false);
}
void ShareDialog::sendClickedByKey(){
    ui->ResponceByKey->setVisible(true);
    ui->ResponceByKey->setText("Response");

}
void ShareDialog::sendClickedByEmail(){
    ui->ResponceByEmail->setVisible(true);
    ui->ResponceByEmail->setText("Response");
}

ShareDialog::~ShareDialog()
{
    delete ui;
}
