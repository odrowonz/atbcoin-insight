#ifndef BONUSCODETAB_H
#define BONUSCODETAB_H

#include "bonuscodedialog.h"
#include "platformstyle.h"
#include <QWidget>
#include <QStandardItemModel>
namespace Ui {
class BonusCodeTab;
}

class BonusCodeTab : public QWidget
{
    Q_OBJECT

public:
    explicit BonusCodeTab(const PlatformStyle *platformStyle,QWidget *parent = 0);
    ~BonusCodeTab();

private:
    Ui::BonusCodeTab *ui;
    const PlatformStyle *platformStyle;
    QStandardItemModel *model;
private Q_SLOTS:
    void updateBonusList();
    void CreateClick(bool);
};

#endif // BONUSCODETAB_H
