#include "include/interface/unsealpasswdui.h"
#include "ui_unsealpasswdui.h"


UnsealPasswdUI::UnsealPasswdUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UnsealPasswdUI)
{
    ui->setupUi(this);
}

UnsealPasswdUI::~UnsealPasswdUI()
{
    delete ui;
}

void UnsealPasswdUI::setPasswd(QByteArray passwd, bool* ok)
{
    this->passwd = passwd;
    this->ok = ok;
}

void UnsealPasswdUI::on_pushButton_clicked()
{
    QByteArray tempHash;
    QString tempPasswd;
    tempPasswd = ui->lineEdit->text();
    Calculator* calculator = new Calculator();
    tempHash =  calculator->sha512(tempPasswd.toStdString()) + '\n';
    if(tempHash == passwd)
    {
        *ok = true;
        this->close();
    }
    else
    {
        QMessageBox message;
        message.setWindowTitle("错误提示");
        message.setText("密码错误。");
        message.setDefaultButton(QMessageBox::Ok);
        message.exec();
    }
}

void UnsealPasswdUI::on_pushButton_2_clicked()
{
    this->close();
    *ok = false;
}
