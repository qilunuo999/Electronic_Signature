#include "include/interface/electronicseal.h"
#include"ui_electronicseal.h"

ElectronicSeal::ElectronicSeal(QWidget *parent, BusinessModel* businessModel):
    QDialog (parent),
    ui(new Ui::ElectronicSeal)
{
    ui->setupUi(this);
    this->businessModel = businessModel;
}

ElectronicSeal::~ElectronicSeal()
{
    delete ui;
}

void ElectronicSeal::setInfo(QString pdfFile, QVector<QImage>** imageList, QScrollArea* tempStorage , int x , int y , int pageNum)
{
    this->pdfFile = pdfFile;
    this->x = x;
    this->y = y;
    this->pageNum = pageNum;
    this->imageList = imageList;
    this->tempStorage = tempStorage;
}

void ElectronicSeal::on_ensureButton_clicked()
{
    if(file.isEmpty() || ui->passwd->text().isEmpty())
    {
        QMessageBox::information(this,"警告","每一项都不可以留空。");
        return;
    }
    businessModel->seal(imageList, tempStorage, x, y, pdfFile, ui->pictrue->text(), ui->passwd->text(), pageNum);
    this->close();
}

void ElectronicSeal::on_cancelButton_clicked()
{
    this->close();
}

void ElectronicSeal::on_pushButton_clicked()
{
    file = QFileDialog::getOpenFileName(this,tr("选择印章"),nullptr,tr("*.jpg *.jpeg *.png"));
    ui->pictrue->setText(file);
}
