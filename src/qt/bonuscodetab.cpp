#include "bonuscodetab.h"
#include "ui_bonuscodetab.h"
#include "walletmodel.h"

BonusCodeTab::BonusCodeTab(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BonusCodeTab)
{
    this->platformStyle=platformStyle;
    ui->setupUi(this);
    ui->tableView->setModel(model=new QStandardItemModel(0,3));
    model->setHorizontalHeaderLabels(QStringList()<<tr("KeyWord")<<tr("Transaction hash")<<tr("number of output"));
    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick(bool)));
}
void BonusCodeTab::updateBonusList(){
}
void BonusCodeTab::CreateClick(bool){
     (new BonusCodeDialog(platformStyle,this))->exec();
     updateBonusList();
}
BonusCodeTab::~BonusCodeTab()
{
    delete ui;
}
