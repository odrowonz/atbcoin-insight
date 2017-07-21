#ifndef PREVIEWCODEDIALOG_H
#define PREVIEWCODEDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QSortFilterProxyModel>
namespace Ui {
class PreviewCodeDialog;
}

class PreviewCodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewCodeDialog(QSortFilterProxyModel* idx,int row,QWidget *parent = 0);
    ~PreviewCodeDialog();

private:
    Ui::PreviewCodeDialog *ui;
};

#endif // PREVIEWCODEDIALOG_H
