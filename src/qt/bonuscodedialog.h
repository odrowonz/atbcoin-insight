#ifndef BONUSCODEDIALOG_H
#define BONUSCODEDIALOG_H
/*
 * Разработать формы для создания бонусных кодов.
 * Пользователь вводит количество монет, ключевое слово, подтверждает отправку.
 * После этого должна создаваться и отправляться в сеть транзакция,
 * имеющая аутпут с кол-вом монет, введенным пользователем и scriptPubKey имеющим вид:
 *  OP_HASH160 ripemd160(ключевое слово) OP_EQUAL.
 * Кнопку вызова формы можно поместить в меню.
 * ------------------------------------------------------------------------------------------------
 * Создать вкладку на главном экране кошелька, в которой будут отображаться
 * созданные бонусные коды (ключевые слова) + инфо(хэш транзакции + номер аутпута в котором
 * он был создан).
 * Пользователь не вводит ключевое слово.
 *  Мы генерируем случайный код вида - ATB-3EIOPJ4A-JMWUIGPV-NR76TESB-UYWH1UJS-HWJOXSWK
 *
*/
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
    explicit BonusCodeDialog(const PlatformStyle *platformStyle,QWidget *parent = 0);

    ~BonusCodeDialog();

private:
    Ui::BonusCodeDialog *ui;
    const PlatformStyle *platformStyle;
private Q_SLOTS:
    void CreateClick();
};

#endif // BONUSCODEDIALOG_H
