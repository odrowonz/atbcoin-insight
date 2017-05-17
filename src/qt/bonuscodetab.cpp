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
    model->setHorizontalHeaderLabels(QStringList()<<tr("KeyWord")<<tr("Transaction hash")<<tr("number of output"));
    connect(ui->BCreate,SIGNAL(clicked(bool)),this,SLOT(CreateClick(bool)));
}
void BonusCodeTab::updateBonusList(){
    CWallet::InitLoadWallet();
    CWallet *wallet=pwalletMain;
    Bonusinfoset list;
    wallet->GetListOfBonusCodes(&list);
    model->clear();
    int row=0;
    for(Bonusinfoset::iterator i=list.begin();i!=list.end();i++){
        std::cout<<i->key<<std::endl;
        std::cout<<i->hashTx.ToString()<<std::endl;
        std::cout<<i->nVout<<std::endl;
        //model->setData(model->index(row,0),i->key);
        //model->setData(model->index(row,1),);
        //model->setData(model->index(row,2),i->nVout);
        row++;
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
