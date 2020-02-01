#include "include/interface/unsealui.h"
#include "ui_unsealui.h"

UnsealUI::UnsealUI(QWidget *parent) :
    QDialog (parent),
    ui(new Ui::UnsealUI)
{
    ui->setupUi(this);
    ok = new bool;
    ok = false;
    picture = new QVector<QByteArray>();
    page = new QVector<QByteArray>();
    sealId = new QVector<QByteArray>();
    passwd = new QVector<QByteArray>();
}

UnsealUI::~UnsealUI()
{
    delete ui;
}

void UnsealUI::setPdfFileName(QString pdfFileName)
{
    this->pdfFileName = pdfFileName;
}

void UnsealUI::setSealUpdate(QVector<QVector<QByteArray>>* sealUpdate)
{
    this->sealUpdate = sealUpdate;
}

void UnsealUI::setCertification(QVector<QVector<QByteArray>>* certification)
{
    this->certification = certification;
}

void UnsealUI::setIsOK(bool* isOK)
{
    this->isOK = isOK;
}

void UnsealUI::setFileSize(qint64 size)
{
    this->size = size;
}

void UnsealUI::setPos(qint64 pos)
{
    this->pos = pos;
}

void UnsealUI::analyseSeal()
{
    extractPicture();
    extractSealID();
    extractSealPage();
    extractSealPasswd();
    loadSealList();
}

void UnsealUI::on_pushButton_clicked()
{
    int temp, temp2 = 0, i;
    if(ui->label_3->text().isEmpty())
    {
        QMessageBox message;
        message.setWindowTitle("错误提示");
        message.setText("请先选择印章。");
        message.setDefaultButton(QMessageBox::Ok);
        message.exec();
    }
    for(i = 0; i < ui->listWidget->count(); i++)
    {
        if(ui->listWidget->item(i)->isSelected())
        {
            temp2 = i;
        }
    }
    temp = ui->label_3->text().toInt();
    UnsealPasswdUI* unsealPasswdUi = new UnsealPasswdUI();
    unsealPasswdUi->setPasswd(passwd->at(temp2), &ok);
    unsealPasswdUi->exec();
    if(ok)
    {
        qDebug()<<"密码正确";
        clearSeal(temp);
        *isOK = true;
    }
}

void UnsealUI::on_pushButton_2_clicked()
{
    this->close();
}

void UnsealUI::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QList<QString>temp;
    int temp2;

    temp = item->text().split(' ');
    temp2 = temp.at(1).toInt() - 1;
    ui->label_3->setText(sealId->at(temp2));
    ui->label_5->setText(page->at(temp2));
    QSize size = ui->label_7->size();
    QImage image;
    image.loadFromData(picture->at(temp2));
    QImage tempImage = image.scaled(size, Qt::IgnoreAspectRatio);
    QPixmap* pixmap=new QPixmap();
    pixmap->convertFromImage(tempImage);
    ui->label_7->setPixmap(*pixmap);
    ui->label_7->setAlignment(Qt::AlignCenter);
}

void UnsealUI::extractPicture()
{
    QByteArray temp = "";//临时存储单张印章图片信息
    int temp2 = 0;//用计数的方式寻找图片数据
    for(int i = 0; i < sealUpdate->size(); i++)
    {
        if(sealUpdate->at(i).at(0).at(sealUpdate->at(i).at(0).size()  - 3) == 'y')//获取印章是否可用的标志
        {
            for(int j = 0; j < sealUpdate->at(i).size(); j++)
            {
                if(sealUpdate->at(i).at(j) == "stream\n")
                {
                    temp2++;
                    j++;
                    temp2 %= 2;
                    while (sealUpdate->at(i).at(j) != "endstream\n" && temp2 == 0)
                    {
                        temp += sealUpdate->at(i).at(j);
                        j++;
                    }
                    if(temp2 == 0)
                    {
                        picture->append(temp);
                        temp.clear();
                        break;
                    }
                }
            }
        }
    }
}

void UnsealUI::extractSealID()
{
    QByteArray temp = "";
    QList<QByteArray> temp2;
    for(int i = 0; i < sealUpdate->size(); i++)
    {
        temp2 = sealUpdate->at(i).at(0).split(' ');//获取每一个印章操作的第一行信息
        if(temp2.at(1) != "n" && temp2.at(3) == "y")//获取印章是否可用的标志
        {
            temp = temp2.at(1);
            //int id = temp.toInt() - 1;
            sealId->append(temp);
        }
    }
}

void UnsealUI::extractSealPage()
{
    QByteArray temp = "";
    QList<QByteArray> temp2;
    int temp3;
    for(int i = 0; i < sealUpdate->size(); i++)
    {
        if(sealUpdate->at(i).at(0).at(sealUpdate->at(i).at(0).size()  - 3) == 'y')//获取印章是否可用的标志
        {
            temp2 = sealUpdate->at(i).at(0).split(' ');
            temp = temp2.at(2);
            temp3 = temp.toInt() + 1;
            page->append(QByteArray::number(temp3));
        }
    }
}

void UnsealUI::extractSealPasswd()
{
    QByteArray temp = "";
    int temp2 = 0;
    QList<QByteArray> temp3;
    for(int i = 0; i < sealUpdate->size(); i++)
    {
        temp3 = sealUpdate->at(i).at(0).split(' ');
        if(temp3.at(1).toInt() != 0 && temp3.at(3) == "y")//获取印章是否可用的标志
        {
            temp2 = sealUpdate->at(i).indexOf("%%EOF\n");
            temp = sealUpdate->at(i).at(temp2 + 4);
            passwd->append(temp);
        }
    }
}

void UnsealUI::clearSeal(int id)
{
    Unseal* unseal = new Unseal();
    unseal->setSealId(id);
    unseal->setSealInfo(sealUpdate);
    unseal->setCertification(certification);
    unseal->setFileSize(size);
    unseal->alterSealSign();
    unseal->constructUnsealInfo();
    QVector<QVector<QByteArray>> * unsealUpdate;
    unsealUpdate = unseal->getUnsealInfo();
    PdfWriter* writer = new PdfWriter();
    writer->writeUnseal(pdfFileName, unsealUpdate);
    picture->clear();
    sealId->clear();
    page->clear();
    passwd->clear();
    ui->listWidget->clear();
    extractPicture();
    extractSealID();
    extractSealPage();
    extractSealPasswd();
    loadSealList();
}

void UnsealUI::loadSealList()
{
    int temp = 1;
    QList<QByteArray> temp2;
    for(int i = 0; i < sealUpdate->size(); i++)
    {
        temp2 = sealUpdate->at(i).at(0).split(' ');
        if(temp2.at(1) !="n" && temp2.at(3) == "y")//获取印章是否可用的标志
        {
            QListWidgetItem *item = new QListWidgetItem;
            item->setText("电子印章 " + QString::number(temp) + " 号");
            ui->listWidget->addItem(item);
            temp ++;
        }
    }
}
