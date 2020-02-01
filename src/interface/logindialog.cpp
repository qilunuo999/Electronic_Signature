#include "include/interface/logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

/*登录按钮*/
void LoginDialog::on_pushButton_clicked()
{
    QString account = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();

    if(account.trimmed().isEmpty() || password.trimmed().isEmpty())
    {
        ui->report->setText("任何一项都不能为空。");
        return;
    }

    Network* net = new Network();
    connect(net,SIGNAL(finish(QJsonObject)),this,SLOT(success(QJsonObject)));
    net->sendRequest(QUrl("http://holer.cc:50356/user/loginClient?account="+account+"&password="+password));
}

/*注册按钮*/
void LoginDialog::on_pushButton_2_clicked()
{
    QDesktopServices::openUrl(QUrl("http://holer.cc:50356"));
}

void LoginDialog::success(QJsonObject jsonObject)
{
    if(!jsonObject.value("error").toString().isEmpty())
    {
        QMessageBox::warning(this,QObject::tr("错误"),QObject::tr(jsonObject.value("error").toString().toLocal8Bit()),QMessageBox::Abort);
        return;
    }
    emit(loginSuccess(jsonObject));
    this->close();
}
