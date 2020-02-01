#ifndef ELECTRONICSEAL_H
#define ELECTRONICSEAL_H

#include<QDialog>
#include<QScrollArea>
#include<QFileDialog>
#include"include/controller/businessmodel.h"
//签章密码验证窗口
namespace Ui
{
    class ElectronicSeal;
}

class ElectronicSeal : public QDialog
{
    Q_OBJECT
public:
    explicit ElectronicSeal(QWidget *parent = nullptr, BusinessModel* businessModel = nullptr);
    void setInfo(QString pdfFile = nullptr, QVector<QImage>** imageList = nullptr, QScrollArea* tempStorage = nullptr, int x = 0, int y = 0, int pageNum = 0);
    ~ElectronicSeal();

private slots:


    void on_ensureButton_clicked();

    void on_cancelButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::ElectronicSeal* ui;
    QString file;
    QString pdfFile;
    QString priKey;
    int pageNum;
    int x;
    int y;
    QVector<QImage>** imageList;
    QScrollArea* tempStorage;
    BusinessModel* businessModel;
};

#endif // ELECTRONICSEAL_H
