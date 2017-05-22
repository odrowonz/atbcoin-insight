#ifndef BONUSCODEDIALOG_H
#define BONUSCODEDIALOG_H
#include <QDialog>
#include "platformstyle.h"
#define KEY_TEMPLATE "3EIOPJ4A-JMWUIGPV-NR76TESB-UYWH1UJS-HWJOXSWK"
namespace Ui {
class BonusCodeDialog;
}

class BonusCodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BonusCodeDialog(QWidget *parent = 0);

    ~BonusCodeDialog();

private:
    Ui::BonusCodeDialog *ui;
private Q_SLOTS:
    void CreateClick();
};

#endif // BONUSCODEDIALOG_H
