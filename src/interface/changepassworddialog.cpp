#include "include/interface/changepassworddialog.h"
#include "ui_changepassworddialog.h"

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePasswordDialog)
{
    ui->setupUi(this);
}

ChangePasswordDialog::~ChangePasswordDialog()
{
    delete ui;
}

/*提交修改的密码*/
void ChangePasswordDialog::on_pushButton_clicked()
{
    if(ui->lineEdit->text().trimmed().isEmpty() || ui->lineEdit_2->text().trimmed().isEmpty() || ui->lineEdit_3->text().trimmed().isEmpty())
    {
        ui->report->setText("任何一项都不能为空。");
        return;
    }

    if(ui->lineEdit_2->text() != ui->lineEdit_3->text())
    {
        ui->report->setText("确认密码与新密码不相同。");
        return;
    }
    Network* net = new Network();
    connect(net,SIGNAL(finshi(QJSONobject)),this,SLOT(changeSuccess()));
    net->sendRequest(QUrl("http://holer.cc:50356/user/changePassword?oldPassword="+ui->lineEdit->text()+"&newPassword="+ui->lineEdit_2->text()));
}

/*修改完毕*/
void ChangePasswordDialog::changeSuccess(QJsonObject jsonObject)
{
    if(jsonObject.value("error").toString().isEmpty())
    {
        QMessageBox::warning(this,QObject::tr("错误"),QObject::tr(jsonObject.value("error").toString().toLocal8Bit()),QMessageBox::Abort);
        return;
    }
    QMessageBox::information(this,QObject::tr("修改结果"),QObject::tr("密码修改成功"),QMessageBox::Abort);
    this->close();
}
