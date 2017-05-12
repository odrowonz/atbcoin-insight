#include "bonuscodedialog.h"
#include "ui_bonuscodedialog.h"

BonusCodeDialog::BonusCodeDialog(const PlatformStyle *platformStyle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BonusCodeDialog)
{
    this->platformStyle=platformStyle;
    ui->setupUi(this);
    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick()));
    connect(ui->BCancel,SIGNAL(clicked(bool)),this,SLOT(close()));
}
void BonusCodeDialog::CreateClick(){

}
BonusCodeDialog::~BonusCodeDialog()
{
    delete ui;
}
