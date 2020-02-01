#ifndef UNSEALPASSWDUI_H
#define UNSEALPASSWDUI_H

#include <QDialog>
#include <QMessageBox>
#include "include/utils/calculator.h"

namespace Ui {
class UnsealPasswdUI;
}

class UnsealPasswdUI : public QDialog
{
    Q_OBJECT

public:
    explicit UnsealPasswdUI(QWidget *parent = nullptr);
    //装载密码
    void setPasswd(QByteArray passwd, bool* ok);
    ~UnsealPasswdUI();


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::UnsealPasswdUI *ui;
    QByteArray passwd;
    bool* ok;
};

#endif // UNSEALPASSWDUI_H
