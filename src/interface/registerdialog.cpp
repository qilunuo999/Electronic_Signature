#include "include/interface/registerdialog.h"
#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

/*注册*/
void RegisterDialog::on_pushButton_clicked()
{
    if(ui->account->text().trimmed().isEmpty() || ui->password->text().isEmpty() || ui->repasswd->text().trimmed().isEmpty() || ui->username->text().trimmed().isEmpty() || ui->cellphone->text().trimmed().isEmpty())
    {
        ui->report->setText("任何一项都不能为空。");
        return;
    }
    if(ui->password->text() != ui->repasswd->text())
    {
        ui->report->setText("确认密码与密码不相同！");
        return;
    }

    Network net;
    connect(&net,SIGNAL(finshi(QJSONobject)),this,SLOT(changeSuccess()));
    net.__init__(QUrl("http://holer50143.restclient.cn/user/register"));
    QString data("account="+ui->account->text()+"&password="+ui->password->text()+"&username="+ui->username->text()+"&cellphone="+ui->cellphone->text());
    net.sendRequest(data.toLocal8Bit());
}

void RegisterDialog::registerSuccess(QJsonObject jsonObject)
{
    if(jsonObject.value("error").toString().isEmpty())
    {
        QMessageBox::warning(this,QObject::tr("错误"),QObject::tr(jsonObject.value("error").toString().toLocal8Bit()),QMessageBox::Abort);
        return;
    }
    QMessageBox::information(this,QObject::tr("注册结果"),QObject::tr("注册成功"),QMessageBox::Abort);
    this->close();
}
