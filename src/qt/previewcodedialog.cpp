#include "previewcodedialog.h"
#include "ui_previewcodedialog.h"

PreviewCodeDialog::PreviewCodeDialog(QSortFilterProxyModel* idx,int row,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewCodeDialog)
{
    ui->setupUi(this);
    ui->AmountFirst->setText(idx->headerData(1,Qt::Horizontal).toString()+":");
    ui->CodeFirst->setText(idx->headerData(3,Qt::Horizontal).toString()+":");
    ui->HashFirst->setText(idx->headerData(2,Qt::Horizontal).toString()+":");
    ui->DateFirst->setText(idx->headerData(0,Qt::Horizontal).toString()+":");

    ui->AmountSecond->setText(QString::number(idx->data(idx->index(row,1)).toDouble(),'f',3));
    ui->CodeSecond->setText(idx->data(idx->index(row,3)).toString());
    ui->HashSecond->setText(idx->data(idx->index(row,2)).toString());
    ui->DateSecond->setText(idx->data(idx->index(row,0)).toString());
}

PreviewCodeDialog::~PreviewCodeDialog()
{
    delete ui;
}
