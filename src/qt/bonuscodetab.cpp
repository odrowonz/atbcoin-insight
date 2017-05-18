#include "bonuscodetab.h"
#include "ui_bonuscodetab.h"
#include "walletmodel.h"

#include "../wallet/wallet.h"
BonusCodeTab::BonusCodeTab(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BonusCodeTab)
{
    this->platformStyle=platformStyle;
    ui->setupUi(this);
    ui->tableView->setModel(model=new QStandardItemModel(0,3));
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick(bool)));
    updateBonusList();
}
void BonusCodeTab::updateBonusList(){
    CWallet::InitLoadWallet();
    CWallet *wallet=pwalletMain;
    model->clear();
    model->setHorizontalHeaderLabels(QStringList()<<tr("nVout")<<tr("Transaction hash")<<tr("KeyWord"));
    ui->tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->tableView->setColumnWidth(0,60);
    for(Bonusinfoset::iterator i=wallet->GetListOfBonusCodes().begin();i!=wallet->GetListOfBonusCodes().end();i++){
        model->insertRow(0);
        model->setData(model->index(0,2),QString::fromStdString(i->key));
        model->setData(model->index(0,1),QString::fromStdString(i->hashTx.ToString()));
        model->setData(model->index(0,0),i->nVout);
    }
}
void BonusCodeTab::CreateClick(bool){
     (new BonusCodeDialog(platformStyle,this))->exec();
     updateBonusList();
}
BonusCodeTab::~BonusCodeTab()
{
    delete ui;
}
