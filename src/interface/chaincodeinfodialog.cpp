#include "include/interface/chaincodeinfodialog.h"
#include "ui_chaincodeinfodialog.h"

ChaincodeInfoDialog::ChaincodeInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChaincodeInfoDialog)
{
    ui->setupUi(this);
}

ChaincodeInfoDialog::~ChaincodeInfoDialog()
{
    delete ui;
}

/*确认按钮，点击关闭*/
void ChaincodeInfoDialog::on_pushButton_clicked()
{
    this->close();
}

void ChaincodeInfoDialog::display(QJsonObject json)
{
    QString temp = ui->textEdit->toPlainText();//保存上一次添加的数据
    //添加新数据
    for(int i = 0; i < json.length(); i++)
    {
        QJsonObject jsonTemp = json.value(QString::number(i)).toObject();
        qDebug()<<jsonTemp;
        temp.append(QString::number(i+1)+"号印章\n");
        temp.append("印章名称："+jsonTemp.value("sealName").toString()+"\n");
        temp.append("盖章用户名称："+jsonTemp.value("username").toString()+"\n");
        temp.append("盖章用户所属公司："+jsonTemp.value("company").toString()+"\n");
        temp.append("印章类型："+jsonTemp.value("sealType").toString()+"\n");
        temp.append("印章信息："+jsonTemp.value("sealInfo").toString()+"\n"+"\n");
    }

    ui->textEdit->setText(temp);
}
