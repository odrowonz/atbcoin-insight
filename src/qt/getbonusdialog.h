#ifndef GETBONUSDIALOG_H
#define GETBONUSDIALOG_H

#include <QDialog>

namespace Ui {
class GetBonusDialog;
}

class GetBonusDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GetBonusDialog(QWidget *parent = 0);
    ~GetBonusDialog();
private:
    Ui::GetBonusDialog *ui;
private Q_SLOTS:
    void OkClick(bool);
};

#endif // GETBONUSDIALOG_H
