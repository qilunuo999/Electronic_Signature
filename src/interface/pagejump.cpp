#include "include/interface/pagejump.h"
#include "ui_pagejump.h"

PageJump::PageJump(QVector<QImage>** imageList, QScrollArea* tempStorage, QLabel * action, int numPages[], QWidget *parent, QString fp):
  QDialog (parent),
  ui(new Ui::PageJump)
{
    ui->setupUi(this);
    this->imageList = imageList;
    this->tempStorage = tempStorage;
    this->action = action;
    this->numPages = numPages;
    this->fp = fp;
}

PageJump::~PageJump()
{
    delete ui;
}

void PageJump::on_pushButton_clicked()
{
    pageNum = ui->textEdit->toPlainText();
    if(pageNum.isEmpty() || pageNum.toInt()<1){
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("输入的页数不能小于1"),QMessageBox::Abort);
        return;
    }
    if(pageNum.toInt()-1 < (*numPages))
    {
        BusinessModel* businessModel = new BusinessModel ();
        businessModel->jumpPage(pageNum.toInt()-1,imageList,tempStorage,fp);
        action->setText("第" + pageNum.append("页/共"+QString::number(numPages[0]))+"页");
    }
    else
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("并没有那么多页......"),QMessageBox::Abort);
    }
}

void PageJump::on_pushButton_2_clicked()
{
    this->close();
}
