#include "bonuscodetab.h"
#include "ui_bonuscodetab.h"
#include "walletmodel.h"
#include "getbonusdialog.h"
#include "../wallet/wallet.h"
#include <QTime>
#include "../main.h"
BonusCodeTab::BonusCodeTab(WalletModel *wmodel_, const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BonusCodeTab)
{
    wmodel=wmodel_;
    this->platformStyle=platformStyle;
    ui->setupUi(this);
    ui->tableView->setModel(model=new QStandardItemModel(0,3));
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick(bool)));
    connect(ui->BGet,SIGNAL(clicked(bool)),this,SLOT(getBonusClick(bool)));
    updateBonusList();
}
void BonusCodeTab::updateBonusList(){
    model->clear();
    model->setHorizontalHeaderLabels(QStringList()<<tr("time")<<tr("nVout")<<tr("Amount")<<tr("Transaction hash")<<tr("KeyWord"));
    ui->tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Stretch);
    ui->tableView->setColumnWidth(0,160);
    ui->tableView->setColumnWidth(1,60);
    ui->tableView->setColumnWidth(2,120);
    for(Bonusinfoset::iterator i=pwalletMain->GetListOfBonusCodes().begin();i!=pwalletMain->GetListOfBonusCodes().end();i++){
        CTransaction tx;
        uint256 hashBlock;
        if(GetTransaction(i->hashTx, tx, Params().GetConsensus(), hashBlock, true)){
            model->insertRow(0);
            model->setData(model->index(0,4),QString::fromStdString(i->key));
            model->setData(model->index(0,3),QString::fromStdString(i->hashTx.ToString()));
            model->setData(model->index(0,2),(tx.vout[i->nVout].nValue/(double)COIN));
            model->setData(model->index(0,1),i->nVout);
            model->setData(model->index(0,0),QDateTime::fromTime_t(tx.nTime).toString());
        }
    }
}
void BonusCodeTab::setWalletModel(WalletModel *wmodel){
    this->wmodel=wmodel;
}
void BonusCodeTab::getBonusClick(bool){
    (new GetBonusDialog(this))->exec();
    wmodel->updateTransaction();
    wmodel->pollBalanceChanged();
}
void BonusCodeTab::CreateClick(bool){
     (new BonusCodeDialog(this))->exec();
     updateBonusList();
}
BonusCodeTab::~BonusCodeTab()
{
    delete ui;
}
